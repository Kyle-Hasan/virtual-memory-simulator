
#include <iostream>
using namespace std;

#include <cstdint>
#include <vector>

template <typename T>
class myVector {
private:
   T *start = NULL;
    
public:
    
    int size = 0;
    myVector<T>(T arr[], int s);
    myVector<T>();
    ~myVector<T>();
    myVector<T>(T initial,int s);
    void print();
    void push_back(T addOn);
    T pop();
    void insert(T addOn, int index);
    void erase(int index);
    T* begin();
    T* end();
    T operator[](int index);

};
template <typename T>
void myVector<T>::print() {
    for (int i = 0; i < size; i++)
        cout<<" "<<*(start + i);
    cout<<endl;
}
template <typename T>
myVector<T>::myVector(T arr[], int s){
    start = new T[s];
    size = s;


}
template <typename T>
myVector<T>::myVector(){

}
template <typename T>
myVector<T>::~myVector(){
    if(start){
    delete[] start;
    }
}
template <typename T>
void myVector<T>::push_back(T addOn){
    size++;
   
    T* temp = new T[size];
    for(int i = 0 ; i < size-1; i++){
        temp[i] = start[i];

    }
    temp[size-1] = addOn;

    delete[] start;
    start = temp;
}
template <typename T>
T myVector<T>::pop(){
if(size == 0){
    return NULL;
}
 size--;
 T returnValue = *(start+size);
 T* temp = new T[size];
 for(int i = 0; i < size; i++){
     temp[i] = start[i];
 }
 delete[] start;
 start = temp;
 return returnValue;
 


}
template <typename T>
void myVector<T>::insert(T addon, int index){
    size++;
    T * temp = new T[size];
    temp[index] = addon;
    int i = 0;
    while(i < index){
        temp[i] = start[i];

        i++;
    }
    while(i < size-1){
        temp[i+1] = start[i];
        i++;

    }



    delete[] start;
    start = temp;

}
template <typename T>
void myVector<T>::erase(int index){
  size--;
   T * temp = new T[size];
    
    int i = 0;
    while(i < index){
        temp[i] = start[i];

        i++;
    }
    i++;
    while(i < size+1){
        temp[i-1] = start[i];
        i++;

    }



    delete[] start;
    start = temp;


}
template <typename T>
T myVector<T>::operator[](int index){

  return start[index];
}
