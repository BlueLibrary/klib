#include "trie.h"


namespace klib{
namespace algo{



Trie::Trie()
{
    root = new Trie_node();
}

void Trie::insert(const char* word)
{
    Trie_node *location = root;
    while(*word)
    {
        if(location->next[*word-'a'] == NULL)//����������
        {
            Trie_node *tmp = new Trie_node();
            location->next[*word-'a'] = tmp;
        }    
        location = location->next[*word-'a']; //ÿ����һ�����൱����һ���´�������ָ��Ҫ�����ƶ�
        word++;
    }
    location->isStr = true; //����β��,���һ����
}

Trie::Trie_node* Trie::searchLocation(const char *word)
{
    Trie_node *location = root;
    while(*word && location)
    {
        location = location->next[*word-'a'];
        word++;
    }
    return location;
}

bool Trie::search(const char *word)
{
    Trie_node *location = root;
    while(*word && location)
    {
        location = location->next[*word-'a'];
        word++;
    }
    return (location!=NULL && location->isStr);
}

void Trie::output(Trie_node * location, const char * word)
{
    if(location->isStr)
    {
        cout<<word<<endl;
    }

    for(int index = 0; index < branchNum; index++)
    {
        if(location->next[index] != NULL)
        {
            char * tmp = new char[strlen(word)+2];
            strcpy(tmp,word);
            tmp[strlen(word)] = 'a'+index;
            tmp[strlen(word)+1] = '\0';
            output(location->next[index], tmp);
            delete tmp;
        }
    }
}

void Trie::deleteTrie(Trie_node *root)
{
    for(int i = 0; i < branchNum; i++)
    {
        if(root->next[i] != NULL)
        {
            deleteTrie(root->next[i]);
        }
    }
    delete root;
}


bool Trie::deleteWord(const char* word)  
{  
    Trie_node * current = root;  
    std::stack<Trie_node*> nodes;//������¼�������м��㣬���Ժ����¶��ϵ�ɾ��  
    while (*word != '\0' && current != 0)  
    {  
        nodes.push(current);//�������м���ѹջ  
        current = current->next[*word - 'a'];  
        word++;
    }  

    if (current && current->isStr)  
    {  
        current->isStr = false;//��ʱcurrentָ���word��Ӧ��Ҷ�ӽ��  
        while (nodes.size() != 0)  
        {  
            char c = *(--word);  
            current = nodes.top()->next[c - 'a'];//ȡ�õ�ǰ����Ľ��  

            bool isNotValid = true;  
            for (int i=0;i<26;++i)//�жϸý���Ƿ������е�nextΪ�ջ�ֻ�и�word��Ӧ�ַ���Ϊ��  
            {  
                if (current->next[i] != 0)  
                {  
                    isNotValid = false;  
                }  
            }  
            if (current->isStr == 0 && isNotValid)//��һ���ڵ���Ч������ֻ�͸�word��أ�����ɾ��  
            {  
                delete current;  
            }  
            else//˵����ǰ����м���Ҳ�������Ľ�����ã�����ɾ����  
            {  
                break;  
            }  
            nodes.top()->next[c - 'a'] = 0;//���ϲ�Ľ���next��ָ��current����ָ����0  
            nodes.pop();  
        }  
        return true;  
    }   
    else 
    {  
        return false;  
    }  
} 

//-----����1------��ע�Ͳ���2--------
// void main() //�򵥲���
// {
//     Trie t;
//     t.insert("abcd");         
//     t.insert("abef");
//     char * c = "acg";
//     t.insert(c);
//     t.insert("acf");
// 
// 	char * test = "ac";
// 	Trie_node * location = t.searchLocation(test);
// 	t.output(location,"ac");
// 	     
// }

//-----����2------��ע�Ͳ���1--------
//void main() //deleteWord �ļ򵥲��� 
//{
//	Trie t;
//	t.insert("abc");
//	t.insert("d");
//	char * c = "dda";
//	t.insert(c);
//	t.insert("da");
//	if(t.search("da"))
//	{
//		printf("find\n");
//	}
//	t.deleteWord("dda");
//	if(!t.search("dda"))
//	{
//		printf("not find dda\n");
//	}
//	t.deleteWord("d");
//	if(!t.search("d"))
//	{
//		printf("not find d\n");
//	}
//	if(t.search("da"))
//	{
//		printf("find\n");
//	}
//}


}}

