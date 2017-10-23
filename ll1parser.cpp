#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <fstream>
#include <stack>
using namespace std;

/**
Author - Anant Kumar Singh
Description : 	Reads a given context free grammar 
				from a text file and build a LL(1) parser 
				out of it by computing first and follow 
				sets to check whether the input 
				string can be parsed from given 
				grammar or not.
*/

//util function to prevent repetitions while inserting
void uniqueInsert(vector<char> &v, char c){
	for(int i=0;i<v.size();++i)
		if(v[i]==c)return;
	v.push_back(c);
}

void printSet(const set<char> &s,bool newline = 1){
	set < char > :: iterator ft;
	for(ft=s.begin();ft!=s.end();++ft)
		cout<<*ft<<' ';
	if(s.size()>0 && newline == 1)cout<<endl;
}

// returns true when first(V) contains ε / ^
// false otherwise
bool First(char V, map<char,vector<string> >& gr, map<char,vector<char> >& F){
	if(F[V].size() > 0){
		for(int i=0;i<F[V].size();++i)if(F[V][i]=='^')return 1;
		return 0;
	}
	if(!isupper(V)){
		F[V].push_back(V);
		return 0;
	}
	//else
	int prods = gr[V].size();
	bool hasnull = 0;
	for(int p=0;p < prods;++p){
		if(gr[V][p] == "^"){
			F[V].push_back('^');
			hasnull = 1;
		}
		else{
			int len = gr[V][p].length();
			for(int i=0;i<len;++i){
				char C = gr[V][p][i];
				bool N = First(C,gr,F);
				// add elements of First(C) in First(V)
				for(int j = 0; j < F[C].size(); ++j)
					uniqueInsert(F[V],F[C][j]);

				// if null is in First(C) continue looking for next symbol
				if(N)continue;
				else break;
			}
		}
	}
	if(F[V].size() == 0){
		F[V].push_back('^');
		hasnull = 1;
	}
	return hasnull;
}

// finds follow of V by searching in all of the grammar
void Follow(char V, map<char,vector<string> >& gr, map<char,vector<char> >& first, map<char,set<char> >& follow){
	// cout<<"Follow of "<<V<<endl;
	map <char,vector < string > >:: iterator it,it2;
	set < char > :: iterator ft;
	for(it = gr.begin();it!=gr.end();++it){
		for(int p=0; p < it->second.size();++p){
			int len = it->second[p].size();
			string s = it->second[p];
			char next;
			for(int i=0;i<len;++i){
				if(s[i]==V){
					// cout<<"found in "<<it->first<<" @ "<<p<<','<<i<<endl;
					g:;
					bool hasnull = 0;
					if(i+1<len){
						next = s[i+1];
						if(isupper(next)!=0){
							// cout<<"next="<<next<<'-'<<first[next].size()<<endl;
							// insert first of s[i+1]
							for(int j=0;j<first[next].size();++j){
								if(first[next][j] == '^')	hasnull = 1;
								else{
									// cout<<"."<<first[next][j]<<endl;
									follow[V].insert(first[next][j]);
								}
							}
							++i;
							if(hasnull){
								// cout<<"first of "<<s[i]<<" had ε\n";
								goto g;
							}
						}
						else{
							// cout<<'.'<<next<<endl;
							follow[V].insert(next);}
					}
					// last in production
					else{
						// insert follow of it->first
						// cout<<"inserting follow of "<<it->first<<'-'<<follow[it->first].size()<<endl;
						for(ft=follow[it->first].begin(); ft != follow[it->first].end(); ++ft)
							if(*ft != '^'){
								// cout<<*ft<<' ';
								follow[V].insert(*ft);
							}
						// cout<<endl;
					}
				}
			}
		}
	}
}
// void getSymbols(map <char,vector < string > >& gr, vector<char>& T, vector<char>& NT){
// 	for(map <char,vector < string > >:: iterator it = gr.begin();it!=gr.end();++it){
// 		uniqueInsert(T,it->first);
// 		int n = it->second.size();
// 		for(int p=0;p<n;++p){
// 			string s = it->second[p];
// 			int len = s.length();
// 			for(int i=0;i<len;++i)
// 				if(s[i] != '^' && !isupper(s[i]))
// 					uniqueInsert(NT,s[i]);
// 		}
// 	}
// 	for(int i=0;i<T.size();++i)cout<<T[i]<<' ';cout<<endl;
// 	for(int i=0;i<NT.size();++i)cout<<NT[i]<<' ';cout<<'$'<<endl;
// }

vector<char> firstOfString(map<char,vector<char> >& first, string prod){
	vector<char> ans;
	int len = prod.length();
	bool hadnull = 1;
	for(int i=0;i<len;++i){
		if(!hadnull) break;
		//else
		hadnull = 0;
		if(isupper(prod[i])){
			for(int j=0;j<first[prod[i]].size();++j){
				if(first[prod[i]][j]=='^')hadnull=1;
				uniqueInsert(ans,first[prod[i]][j]);
			}
		}
		else
			uniqueInsert(ans,prod[i]);
	}
	return ans;
}

#define pcc pair<char,char>
	
void createParseTable(map<pcc,string >&ptable, map <char,vector<string> >& gr, map<char,vector<char> >& first, map<char,set<char> >& follow){
	for(map <char,vector < string > >:: iterator it = gr.begin();it!=gr.end();++it){
		// cout<<"for: "<<it->first<<endl;
		int n = it->second.size();
		for(int p=0;p<n;++p){
			// cout<<it->second[p]<<endl;
			vector<char> v = firstOfString(first,it->second[p]);
			// cout<<"first of "<<it->second[p]<<": ";
			// for(int i=0;i<v.size();++i)cout<<v[i]<<'.';cout<<endl;
			bool null = 0;
			// cout<<"inserting : ";
			for(int i=0;i<v.size();++i)
				if(v[i]!='^')	{
					// cout<<v[i]<<',';
								ptable[make_pair(it->first,v[i])]=it->second[p];}
				else null=1;
			if(null){
				set < char > :: iterator ft,z;
				z = follow[it->first].end();
				// cout<<"\nnull inserting : ";
				for(ft=follow[it->first].begin();ft!=z;++ft)
					// {cout<<*ft<<'_';
					ptable[make_pair(it->first,*ft)]=it->second[p];
					// }
			}
			// cout<<endl;
		}
	}
}

void print(map <char,vector < string > >& gr, map<char,vector<char> >& firstMap, map<char,set<char> >& followMap){
	// Printing grammar
	map <char,vector < string > >:: iterator it;
	map <char,vector < char > > :: iterator it1;
	for(it = gr.begin();it!=gr.end();++it){
		cout<<it->first<<" --> ";
		int n = it->second.size();
		for(int i=0;i<n;++i)
			cout<<it->second[i]<< "\t";
		cout<<endl;
	}
	cout<<endl;
	// Printing First
	for(it=gr.begin();it!=gr.end();it++){
		cout<<"First("<<it->first<<") = { ";
		it1=firstMap.find(it->first);
		int sz = it1->second.size();
		for(int i=0;i<sz-1;++i)	cout<<it1->second[i]<< " , ";
		cout<<it1->second[sz-1]<<' ';
		cout<<"}"<<endl;
	}
	cout<<endl;
	// Printing Follow
	for(it=gr.begin();it!=gr.end();it++){
		cout<<"Follow("<<it->first<<") = { ";
		printSet(followMap[it->first],0);
		cout<<"}"<<endl;
	}
}

void reversePush(string s, stack<char> &st){
	int len = s.length();
	for(int i=len-1;i>=0;--i)
		st.push(s[i]);
}

bool parse(string s, char start, map<pcc,string> & ptable){
	stack<char>st;
	st.push('$');
	st.push(start);
	s+="$";
	int i=0,len = s.length();
	while(i<len)
		if(isupper(st.top())){
			char Top = st.top();
			// cout<<"Top:"<<Top<<endl;
			if(ptable[make_pair(Top,s[i])]=="")return 0;
			//else
			st.pop();
			if(ptable[make_pair(Top,s[i])] != "^"){
				// cout<<"pushing:"<<ptable[make_pair(Top,s[i])]<<endl;
				reversePush(ptable[make_pair(Top,s[i])],st);
			}
			// else cout<<"^ not pushed"<<endl;
		}
		else
			if(s[i]!=st.top())return 0;
			else{
				st.pop();
				++i;
			}
	return 1;
}

int main(int argc,char * argv[]){
	ifstream G;
	G.open(argv[1]);
	string prod,x,a;
	map <char,vector < string > > gr;
	map <char,vector < char > > firstMap;
	map <char,set < char > > followMap;
	char c;
	int n,st,flag=0;
	if(G.is_open()){
		while(!G.eof()){
			G>>c;
			if(isupper(c)){
				G>>x;
				if( x == "->" ){
						G>>prod;
						n=prod.size();st=0;
						// cout<<n<<'_'<<prod<<endl;
						while(st<n){
							a="";
							while(st<n && prod[st]!='|'){
								a += prod[st++];
							}
							st++;
							gr[c].push_back(a);
						}
				}
				else{
					cout<<"grammer is not correct"<<endl;
					flag=1;
					break;
				}
			}
			else if(c=='_')
				break;
			else{
				cout<<"grammer is not correct"<<endl;
				flag=1;
				break;
			}
		}
	}
	else{
		cout<<"File does not exist"<<endl;
		flag=1;	
	}
	char start = gr.begin()->first;
	G.close();
	if(flag) return 0;

	map <char,vector < string > > :: iterator it;
	for(it=gr.begin();it!=gr.end();it++)
		First(it->first,gr,firstMap);
	followMap[start].insert('$');
	int times = 2;
	while(times--)
	for(it=gr.begin();it!=gr.end();++it)
		Follow(it->first,gr,firstMap,followMap);
	// T.push_back('$');
	// Printing grammar, first and follow
	print(gr,firstMap,followMap);
	//Parse Table construction
	map<pair<char,char>,string> parseTable;
	createParseTable(parseTable,gr,firstMap,followMap);
	cout<<"Enter number of strings to be checked : ";
	int tests;
	cin>>tests;
	while(tests--){
		cout<<"Enter the string to be checked : ";
		string in;
		cin>>in;
		if(parse(in,start,parseTable))cout<<"input string parsed successfully!\n";
		else cout<<"input string cannot be parsed!\n";
	}

}





