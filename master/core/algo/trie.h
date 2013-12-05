#include <iostream>
#include <stack>
using namespace std;

namespace klib {
namespace algo {



const int branchNum = 26; //�������� 


class Trie
{
public:
    struct Trie_node
    {
        bool isStr; //��¼�˴��Ƿ񹹳�һ������
        Trie_node *next[branchNum];//ָ�����������ָ��,�±�0-25����26�ַ�
        Trie_node():isStr(false)
        {
            memset(next,NULL,sizeof(next));
        }
    };

public:
    Trie();

    Trie_node * getRoot(){return root;}
    void insert(const char* word);
    bool search(const char* word);  // ����word�Ƿ����
    Trie_node* searchLocation(const char *word); // ����wordλ��
    void output(Trie_node *location, const char * word);
    void deleteTrie(Trie_node *root);
    bool deleteWord(const char* word);

private:
    Trie_node* root;
};



}}
