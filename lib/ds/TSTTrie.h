#ifndef OPLIB_TSTTRIE_H
#define OPLIB_TSTTRIE_H

#include <utility>
#include <string>
#include <vector>
#include <memory>

#include "alloc.H"
#include "memutil.H"

namespace oplib
{
  template <typename T>
  struct TSTNode
  {
     char _c;
     T* _value;
     TSTNode *_left, *_mid, *_right;
  };

  template <typename Value, template <typename> class Alloc = opallocator >
  class TSTTrie
  {
   public:
    using ValueType = Value;
    using ReturnType = std::pair<ValueType, bool>;

   protected:
    using NodeType = TSTNode<ValueType>;
    using NodePtr = NodeType*;
    using NodeAlloc = Alloc<NodeType>;

    NodeAlloc _allocator;

    NodePtr getNode() { return _allocator.allocate(1); } 

    NodePtr createNode(char c_, const ValueType& val_)
    {
      auto ptr = getNode();
      ptr->_c = c_;
      ptr->_left = ptr->_mid = ptr->_right = nullptr;
      ptr->_value = new ValueType(val_);
      return ptr;
    }

    NodePtr createNode(char c_)
    {
      auto ptr = getNode();
      ptr->_c = c_;
      ptr->_left = ptr->_mid = ptr->_right = nullptr;
      ptr->_value = nullptr;
      return ptr;
    } 

    void destroyNode(NodePtr np_) 
    {
      if (np_->_value != nullptr)
        delete np_->_value; 
      np_->_value = nullptr;

      putNode(np_);
    }

    void putNode(NodePtr np_) 
    {
      _allocator.deallocate(np_, 1); 
    }

    NodePtr get(NodePtr cur_, const std::string& key_, size_t pos_) const;

    void put(NodePtr& cur_, const std::string& key_, const ValueType& val_, size_t pos_);

    void collect(NodePtr node_, std::string pre_, std::vector<std::string>& ret_) const;

    void search(NodePtr node_, const std::string& key_, std::string& curChars,
                std::string& prefix_, size_t pos_) const;

    bool keyExist(NodePtr ptr) const { return ptr->_value != nullptr; }

    void destroy(NodePtr ptr);

    NodePtr copyNode(const NodePtr node_);

   public:
    TSTTrie() : _header(nullptr) {}

    ~TSTTrie() { destroy(_header); }

    TSTTrie(const TSTTrie& rhs_);

    TSTTrie& operator = (const TSTTrie& rhs_);

    ReturnType get(const std::string& key_) const
    {
      if (key_.empty()) return std::make_pair(ValueType(), false);

      auto nodep = get(_header, key_, 0);
      if (nodep == nullptr) return std::make_pair(ValueType(), false);

      if (keyExist(nodep))
        return std::make_pair(ValueType(*(nodep->_value)), true);
      else
        return std::make_pair(ValueType(), false);
    }

    bool contains(const std::string& key_) const
    {
      return get(key_).second;
    }

    void put(const std::string& key_, const ValueType& val_)
    {
      if (key_.empty()) return;
      put(_header, key_, val_, 0);
    }

    /**
     * Return all keys() stored in this Trie
     * as a vector<string>
     */
    std::vector<std::string> keys() const
    {
      std::vector<std::string> ret;
      collect(_header, "", ret);

      return ret;
    }

    /**
     * Return the longest prefix of the key_ that 
     * is stored in this Trie.
     * e.g Goo and Good are stored, then longestPrefixOf("Goody") is "Good"
     */
    std::string longestPrefixOf(const std::string& key_) const
    {
      std::string lpre;
      std::string charsAllAlong;
      search(_header, key_, charsAllAlong, lpre, 0);
      return lpre;
    }

    /**
     * Return all keys stored in this Trie that has prefix_
     * as its prefix
     */
    std::vector<std::string> keysWithPrefix(const std::string& prefix_) const
    {
      std::vector<std::string> ret;
      auto node = get(_header, prefix_, 0);

      // This node match exactly prefix_
      if (node != nullptr)
      {
        if (keyExist(node))
          ret.push_back(prefix_);

        // Three-way collect of all keys with prefix_
        collect(node->_mid, prefix_, ret);
      }

      return ret;
    }

   private:
    NodePtr _header;
  };

  template <typename Value, template <typename> class Alloc>
  typename TSTTrie<Value, Alloc>::NodePtr
  TSTTrie<Value, Alloc>::get(NodePtr ptr_, const std::string& key_, size_t pos) const
  {
    if (ptr_ == nullptr) return nullptr;
    if (key_.length() == 0) return ptr_;

    char c = key_.at(pos);
    if (c == ptr_->_c) 
    {
      // Character matched, go the middle path
      if (pos == key_.length() - 1)
        return ptr_;
      else
        return get(ptr_->_mid, key_, pos + 1);
    }
    else if (c < ptr_->_c)
    {
      // Not matched, char < node's stored character
      return get(ptr_->_left, key_, pos);
    }
    else
    {
      return get(ptr_->_right, key_, pos);
    }
  }

  template <typename Value, template <typename> class Alloc>
  void TSTTrie<Value, Alloc>::put(NodePtr& cur_,
                                  const std::string& key_,
                                  const ValueType& val_,
                                  size_t pos_)
  {
    char c = key_.at(pos_);
    if (cur_ == nullptr)
    {
      cur_ = createNode(c);
    }

    assert(cur_ != nullptr);
    if (c < cur_->_c)
    {
      put(cur_->_left, key_, val_, pos_);
    }
    else if (c > cur_->_c)
    {
      put(cur_->_right, key_, val_, pos_);
    }
    else if (pos_ < key_.length() - 1)
    {
      put(cur_->_mid, key_, val_, pos_ + 1);
    }
    else
    {
      // Reach endpoint, Update value
      if (cur_->_value == nullptr)
        cur_->_value = new ValueType(val_);
      else
        *(cur_->_value) = val_; 
    }
  }

  template <typename Value, template <typename> class Alloc>
  void TSTTrie<Value, Alloc>::collect(NodePtr node_,
                                      std::string pre,
                                      std::vector<std::string>& ret_) const
  {
    if (node_ == nullptr) return;

    auto curKey = pre + node_->_c;
    if (keyExist(node_))
    {
      ret_.push_back(curKey);
    }

    collect(node_->_left, curKey, ret_);
    collect(node_->_mid, curKey, ret_);
    collect(node_->_right, curKey, ret_);
  }

  template <typename Value, template <typename> class Alloc>
  void TSTTrie<Value, Alloc>::search(NodePtr node_,
                                     const std::string& key_,
                                     std::string& curChars_,
                                     std::string& prefix_,
                                     size_t pos_) const
  {
    if (node_ == nullptr) return;
    char c = key_.at(pos_);
    if (c == node_->_c) 
    {
      // Matched, go to mid
      // If this node exists, then record the prefix
      curChars_.push_back(c);
      if (keyExist(node_))
        prefix_ = curChars_;

      search(node_->_mid, key_, curChars_, prefix_, pos_ + 1);
    }
    else if (c < node_->_c)
    {
      search(node_->_left, key_, curChars_, prefix_, pos_);
    }
    else
    {
      search(node_->_right, key_, curChars_, prefix_, pos_);
    }
  }

  template <typename Value, template <typename> class Alloc>
  void TSTTrie<Value, Alloc>::destroy(NodePtr ptr_)
  {
    if (ptr_ == nullptr) return;
    destroy(ptr_->_left);
    destroy(ptr_->_mid);
    destroy(ptr_->_right);
    destroyNode(ptr_);
  }

  template <typename Value, template <typename> class Alloc>
  typename TSTTrie<Value, Alloc>::NodePtr
  TSTTrie<Value, Alloc>::copyNode(const NodePtr node_)
  {
    if (node_ == nullptr) return nullptr;

    NodePtr nd = createNode(node_->_c);
    if (node_->_value != nullptr)
      nd->_value = new ValueType(*(node_->_value));

    nd->_left = copyNode(node_->_left);
    nd->_mid = copyNode(node_->_mid);
    nd->_right = copyNode(node_->_right);

    return nd;
  }

  template <typename Value, template <typename> class Alloc>
  TSTTrie<Value, Alloc>::TSTTrie(const TSTTrie& rhs_)
  {
    _header = copyNode(rhs_._header);
  }

  template <typename Value, template <typename> class Alloc>
  TSTTrie<Value, Alloc>& TSTTrie<Value, Alloc>::operator = (const TSTTrie& rhs_)
  {
    if (this == &rhs_) return *this;

    TSTTrie tmp(rhs_);
    std::swap(_header, tmp._header);
    return *this;
  }
}

#endif
