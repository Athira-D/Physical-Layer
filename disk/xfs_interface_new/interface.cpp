// Code for XFS Interface (for Unix) 

#include<bits/stdc++.h>
#include "imp-exp.cpp"
using namespace std;
vector <string> strip_whitespace(string input_command)
{
	vector <string> words;
	string temp="";
	for(int i=0;i<input_command.length();i++)
	{
		if(input_command[i]==' '||input_command[i]==';')
		{
			if(temp!="")
			{words.push_back(temp);}
			temp="";
		}
		else
		{
			temp=temp+input_command[i];
		}

	}
	if(temp!="")
		words.push_back(temp);
	for(auto i= words.begin();i!=words.end();i++)
	{
		
		cout<<*i<<endl;
	}
	return words;
}

bool check_filenamelength(string filepath)
{
	int pos1= filepath.rfind('.');
	int pos2=filepath.rfind('/');
	int diff=pos2-pos1+1;
	if(diff>15)
	{
		cout<<" File name should have at most 15 characters\n";
		return false;
	}
	else 
		return true;

}


int main()
{
	
	while(1)
	{
		cout<<"# ";
		string input_command;
		//cin.ignore();
		getline(cin,input_command);
		vector <string> s= strip_whitespace(input_command);
		string first=s[0];
		cout<<first<<endl;
		if(first=="EXIT"||first=="exit")
		{
			if(s.size()==1)
				break;
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
		}	
		else if(first=="ls"||first=="LS")
		{
			ls();
		}
		else if(first=="SELECT"||first=="select")
		{
			cout<<"select"<<endl;
		}
		else if(first=="fdisk")
		{
			if(s.size()==1)
			{
				cout<<"fdisk"<<endl;
				createdisk();
				formatdisk();
				meta();
			}
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
			
		}
		else if(first=="import" )
		{
			if(s.size()!=2)
			{
				cout<<"Incorrect number of arguements"<<endl;
			}
			string filepath = s[1];
			char f[filepath.length()];
			int i=0;
			for(i=0;i<filepath.length();i++)
			{
				f[i]=filepath[i];
			}
			f[i]='\0';
			FILE * file=fopen(f,"r");
			if(!file)
			{
				cout<<"Invalid file path or file does not exist"<<endl;
				continue;
			}
			fclose(file);
			bool check= check_filenamelength(filepath);
			if(!check)
			{
				continue;
			}
			cout<<" Calling import"<<endl;
			cout<<f<<endl;
			import(f);

		}
		else if(first=="export")
		{
			// If file name is not given, export to a file with name same as that of relation
			if(s.size()==3)
			{
				char relname[16];
				int i;
				string second=s[1];
				for(i=0;i<15;i++)
					relname[i]=second[i];
				relname[i]='\0';
				string third=s[2];
				char filepath[third.length()];
				for(i=0;i<third.length();i++)
					filepath[i]=third[i];
				filepath[i]='\0';
				exp(relname,filepath);
			}
			else
			{
				cout<<"Incorrect number of arguments"<<endl;
				continue;
			}

		}
		else if(first=="dump")
		{
			if(s.size()!=2)
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
			string second=s[1];
			if(second=="bmap")
			{
				db();
			}
			else if(second=="relcat")
			{
				dump_relcat();
			}
			else if(second=="attrcat")
			{
				dump_attrcat();
			}
		}
		else if(first=="create")
		{

		}
		else if(first=="drop")
		{

		}
		else if(first=="open"||first=="OPEN")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				tablename[i]='\0';
				int x=openRel(tablename);
				if(x==E_RELNOTEXIST)
				{
					cout<<"E_RELNOTEXIST - Relation does not exist"<<endl;
				}
				else if(x==E_CACHEFULL)
				{
					cout<<"E_CACHEFULL - Relation Cache is full"<<endl;
				}
				else
				{
					cout<<" Relation opened successfully"<<endl;
				}

			}
			else
			{
				cout<<"Syntax Error"<<endl;
				continue;
			}
		}
		else if(first=="close"||first=="CLOSE")
		{
			string second=s[1];
			if(second=="TABLE"||second=="table")
			{
				string third=s[2];
				char tablename[16];
				int i;
				for(i=0;i<15;i++)
					tablename[i]=third[i];
				tablename[i]='\0';
				int id=getRelId(tablename);
				if(id==E_RELNOTOPEN)
				{
					cout<<"E_RELNOTOPEN- Relation not open"<<endl;
					continue;
				}
				int x=closeRel(id);
				if(x==E_OUTOFBOUND)
				{
					cout<<"E_OUTOFBOUND - Relation ID is out of bound"<<endl;
				}
				else if(x==E_RELNOTOPEN)
				{
					cout<<"E_RELNOTOPEN- Relation not open"<<endl;
				}
				else
				{
					cout<<" Relation opened successfully"<<endl;
				}

			}
			else
			{
				cout<<"Syntax Error"<<endl;
				
			}
		}
		else if(first=="alter"||first=="ALTER")
		{
			if(s.size()==6)
			{
				if(s[1]=="TABLE"&& s[2]=="RENAME"&&s[4]=="TO")
				{
					string rel=s[3];
					string newrel=s[5];
					char oldrelname[16];
					char newrelname[16];
					int i;
					for(i=0;i<15;i++)
						oldrelname[i]=rel[i];
					oldrelname[i]='\0';
					for(i=0;i<15;i++)
						newrelname[i]=newrel[i];
					newrelname[i]='\0';
					int x=ba_renamerel(oldrelname,newrelname);
					if(x==E_RELEXIST)
					{
						cout<<"Relation with the name already exists"<<endl;
					}
					else if(x==E_RELNOTEXIST)
					{
						cout<<"Relation to be renamed does not exist"<<endl;
					}
					else if(x==SUCCESS)
					{
						cout<<"Renaming done successfully"<<endl;
					}
				}
				else
				{
					cout<<"Syntax Error"<<endl;
				}
			}
			else if(s.size()==8)
			{
				if(s[1]=="TABLE"&& s[2]=="RENAME"&&s[4]=="COLUMN"&&s[6]=="TO")
				{
					string rel=s[3];
					string col=s[5];
					string newcol=s[7];
					char relname[16];
					char oldcolname[16];
					char newcolname[16];
					int i;
					for(i=0;i<15;i++)
						relname[i]=rel[i];
					relname[i]='\0';
					for(i=0;i<15;i++)
						oldcolname[i]=col[i];
					oldcolname[i]='\0';
					for(i=0;i<15;i++)
						newcolname[i]=newcol[i];
					newcolname[i]='\0';
					int x=ba_renameattr(relname,oldcolname,newcolname);
					if(x==E_RELNOTEXIST)
					{
						cout<<"Relation does not exist"<<endl;
					}
					else if(x==E_ATTREXIST)
					{
						cout<<"Attribute with same name exists in the relation"<<endl;
					}
					else if(x==E_ATTRNOTEXIST)
					{
						cout<<"Attribute to be renamed does not exist"<<endl;
					}
					else if(x==SUCCESS)
					{
						cout<<"Renaming done successfully"<<endl;
					}
				}
				else
				{
					cout<<"Syntax Error"<<endl;
				}
			}
			else
			{
				cout<<"Syntax Error"<<endl;
			}
		}
		else
		{
			cout<<"Syntax Error. Type \"help\" for the list of commands"<<endl;
		}



	}
	return 0;
}