#include <stdexcept>
#include <optional>
#include <unordered_map>
#include <memory>
#include <stdexcept>

template<typename Key1, typename Key2, typename Value>
class BiMap {
    private:
        std::unordered_map<Key1, std::shared_ptr<Value>> hash1;
        std::unordered_map<Key2, std::shared_ptr<Value>> hash2;
    public:
        BiMap() = default;
        ~BiMap() = default;

        void Insert(const std::optional<Key1>& key1, 
            const std::optional<Key2>& key2, 
            const Value& value);

        Value& GetByPrimaryKey(const Key1& key);
        const Value& GetByPrimaryKey(const Key1& key) const;
        Value& GetBySecondaryKey(const Key2& key);
        const Value& GetBySecondaryKey(const Key2& key) const;
};

template<typename Key1, typename Key2, typename Value>
void BiMap<Key1, Key2, Value>::Insert(const std::optional<Key1>& key1, 
            const std::optional<Key2>& key2, 
            const Value& value){

    if(!key1 && !key2){
        throw std::invalid_argument("not key");
    }
    if(key1 && hash1.contains(key1.value())){
        throw std::invalid_argument("error 1");
    }
    if(key2 && hash2.contains(key2.value())){
        throw std::invalid_argument("error 2 ");
    }
    auto ptr = std::make_shared<Value>(value);
    if(key1.has_value()){
        hash1[key1.value()] = ptr;
    }
    if(key2.has_value()){
        hash2[key2.value()] = ptr;
    }
    return;
}

template<typename Key1, typename Key2, typename Value>
Value& BiMap<Key1, Key2, Value>::GetByPrimaryKey(const Key1& key){
    const auto it = hash1.find(key);

    if(it == hash1.end()){
        throw std::out_of_range("No such key");
    }
    return *(it->second);
}

template<typename Key1, typename Key2, typename Value>
const Value& BiMap<Key1, Key2, Value>::GetByPrimaryKey(const Key1& key)const{
    const auto it = hash1.find(key);
    if(it == hash1.end()){
        throw std::out_of_range("No such key");
    }
    return *(it ->second);
}



template<typename Key1, typename Key2, typename Value>
Value& BiMap<Key1, Key2, Value>::GetBySecondaryKey(const Key2& key){
    const auto it = hash2.find(key);
    if(it == hash2.end()){
        throw std::out_of_range("No such key");
    }
    return *(it ->second);
}


template<typename Key1, typename Key2, typename Value>
const Value& BiMap<Key1, Key2, Value>::GetBySecondaryKey(const Key2& key)const{
    const auto it = hash2.find(key);
    if(it == hash2.end()){
        throw std::out_of_range("No such key");
    }
    return *(it ->second);
}