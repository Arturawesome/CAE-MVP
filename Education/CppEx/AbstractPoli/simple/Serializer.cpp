#include<vector>
#include<string>
#include<iostream>

class Serializer {
    protected:
        std::vector<int> arr;
    public:
        Serializer() = default;
        virtual ~Serializer() = default;
        virtual void BeginArray() = 0;
        virtual void AddArrayItem(const std::string &) = 0;
        virtual void EndArray() = 0;
};

class JsonSerializer: public Serializer {
    public:
        JsonSerializer(): Serializer() {};
        ~JsonSerializer() override = default;
         void BeginArray()  override;
         void AddArrayItem(const std::string &)  override;
         void EndArray()  override;
};
void JsonSerializer:: AddArrayItem(const std::string & s) {
    if(arr[arr.size() - 1] == 0){
        std::cout<<"\""<<s<<"\"";
        arr[arr.size() - 1]++;
    } else {
        std::cout<<",\""<<s<<"\"";
    }    
}

void JsonSerializer::BeginArray()  {
    if(arr.size() == 0){
        std::cout<<"[";
        arr.push_back(0);
    } else {
         if(arr[arr.size() - 1] == 0){
            std::cout<<"[";
         } else {
            std::cout<<",[";
         }
        
        arr.push_back(0);
    }
}
void JsonSerializer::EndArray()  {
    arr.pop_back();
    std::cout<<"]";
    if (!arr.empty())
        arr.back()++;
}


int main(){
    JsonSerializer j;
    j.BeginArray();
    //j.AddArrayItem("asdf1");
    //j.AddArrayItem("asdf2");
    j.BeginArray();
    //j.AddArrayItem("asdf11");
    //j.AddArrayItem("asdf22");
    j.BeginArray();
    //j.AddArrayItem("asdf111");
    j.EndArray();
     j.AddArrayItem("asdf222");
    j.EndArray();
     j.AddArrayItem("asdf222");
    j.EndArray();

}