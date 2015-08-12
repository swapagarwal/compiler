#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <set>

using namespace std;

struct node{
    node* children[3];
    string node_name;
    string node_val;
    int line_no;
    bool is_int;
    bool is_bool;

    node() {
        is_int=false;
        is_bool=false;
    }
};

class tree{
private:
    ofstream f_tree, mips;
    vector<map<string,string> > sym_table;
    map<string, string> str_map;
    map<string,int> func_arg_map;
    map<string, vector<bool> > func_params;
    set<int> type_mismatch_lines;
    map<string, int> missing_returns;
    int bool_cnt, int_cnt, str_cnt, bool_func_cnt, int_func_cnt, void_func_cnt, label_cnt, tmp_cnt;
    int curr_loop_label;

public:
    tree() {
        f_tree.open("parse_tree.txt", ios_base::trunc);
        mips.open("mips.s", ios_base::trunc);
        bool_cnt = 0;
        int_cnt = 0;
        str_cnt = 0;

        bool_func_cnt = 0;
        int_func_cnt = 0;
        void_func_cnt = 0;

        label_cnt = 0;

        curr_loop_label = -1;

        sym_table.clear();
        sym_table.resize(1);
        str_map.clear();
        func_arg_map.clear();
    }

    node *root;


    string int_to_str (int n)
    {
         stringstream s;
         s << n;
         return s.str();
    }

    node* add_node(string name, string val, int line, node* a=NULL, node* b=NULL, node* c=NULL) {
        node *new_node;
        new_node = new node;

        new_node->children[0] = a;
        new_node->children[1] = b;
        new_node->children[2] = c;
        new_node->node_name=name;
        new_node->node_val=val;
        new_node->line_no=line;

        return new_node;
    }

    void build_sym_table(node *curr_node, bool dec=false, bool func=false) {
        node *x, *y;
        string new_name;


        if(dec && (curr_node->node_name=="INT" || curr_node->node_name=="BOOL")) {
            x=curr_node->children[0];
            if(x!=NULL) {
                if(sym_table[sym_table.size()-1].find(x->node_val)!=sym_table[sym_table.size()-1].end()) {
                    cout<<"variable redeclared in same scope error: '"<<x->node_val<<"' at line number "<<x->line_no<<endl;
                }

                if(curr_node->node_name=="INT") {
                    new_name="i";
                    new_name+=int_to_str(int_cnt++);
                } else {
                    new_name="b";
                    new_name+=int_to_str(bool_cnt++);
                }

                sym_table[sym_table.size()-1][x->node_val]=new_name;
                curr_node->node_val=new_name;
                curr_node->children[0]=curr_node->children[1];
                curr_node->children[1]=curr_node->children[2];
                curr_node->children[2]=NULL;
            }
        }

        if(func && (curr_node->node_name=="INT" || curr_node->node_name=="BOOL" || curr_node->node_name=="VOID")) {
            x=curr_node->children[0];
            string fname="";
            int tmp_cnt;
            if(x!=NULL) {
                if(sym_table[0].find(x->node_val)!=sym_table[0].end()) {
                    cout<<"function redeclared error: '"<<x->node_val<<"' at line number "<<x->line_no<<endl;
                }

                if(curr_node->node_name=="INT") {
                    new_name="if";
                    new_name+=int_to_str(int_func_cnt++);
                    missing_returns.insert(make_pair(new_name, curr_node->line_no));
                } else if(curr_node->node_name=="BOOL") {
                    new_name="bf";
                    new_name+=int_to_str(bool_func_cnt++);
                    missing_returns.insert(make_pair(new_name, curr_node->line_no));
                } else {
                    new_name="vf";
                    new_name+=int_to_str(void_func_cnt++);
                }

                fname=new_name;

                sym_table[0][x->node_val]=new_name;
                curr_node->node_val=new_name;
                curr_node->children[0]=curr_node->children[1];
                curr_node->children[1]=curr_node->children[2];
                curr_node->children[2]=NULL;
            }

            x=curr_node->children[0];

            vector<bool> tmp;
            tmp.clear();

            tmp_cnt=0;
            while(x!=NULL) {
                y=x->children[0];

                if(y->children[0]!=NULL) {
                    if(sym_table[sym_table.size()-1].find(y->children[0]->node_val)!=sym_table[sym_table.size()-1].end()) {
                        cout<<"variable redeclared error: '"<<y->children[0]->node_val<<"' at line number "<<x->line_no<<endl;
                    }

                    if(y->node_name=="INT") {
                        new_name="i";
                        new_name+=int_to_str(int_cnt++);
                        tmp.push_back(true);
                    } else {
                        new_name="b";
                        new_name+=int_to_str(bool_cnt++);
                        tmp.push_back(false);
                    }

                    sym_table[sym_table.size()-1][y->children[0]->node_val]=new_name;
                    func_arg_map[new_name]=tmp_cnt++;
                    y->node_val=new_name;
                    y->children[0]=y->children[1];
                    y->children[1]=y->children[2];
                    y->children[2]=NULL;
                }
                x=x->children[1];
            }
            func_params[fname]=tmp;
        }

        if(curr_node->node_name == "STRING") {
            str_map["s"+int_to_str(str_cnt)]=curr_node->node_val;
            curr_node->node_val="s"+int_to_str(str_cnt++);
            ///sank - str_map["s"+int_to_str(str_cnt++)]=curr_node->node_val;
        }

        bool found_in_scope=false;
        if(curr_node->node_name=="ID") {
            for(int i=sym_table.size()-1; i>=0; i--) {
                if(sym_table[i].find(curr_node->node_val) != sym_table[i].end()) {
                    curr_node->node_val=sym_table[i][curr_node->node_val];
                    found_in_scope=true;
                    break;
                }
            }
            if(!found_in_scope) {
                cout<<"not found in scope error: '"<<curr_node->node_val<<"' not found in scope at line number "<<curr_node->line_no<<endl;
            }
        }

        if(curr_node->node_name=="WHILE" || curr_node->node_name=="IF-THEN" || curr_node->node_name=="IF-THEN-ELSE" || curr_node->node_name=="FUNCTION") {
            sym_table.push_back(map<string,string>());
        }

        bool f=false, g=false;

        if(curr_node->node_name=="DECLARE") {
            f=true;
        }

        if(curr_node->node_name=="FUNCTION") {
            g=true;
        }

        for(int i=0; i<3; i++) {
            if(curr_node->children[i]!=NULL) {
                build_sym_table(curr_node->children[i], f, g);
            }
        }

        if(curr_node->node_name=="DECLARE") {
            x = curr_node->children[0];
            if(x!=NULL) {
                curr_node->node_val=x->node_val;
                curr_node->children[0]=x->children[0];
                curr_node->children[1]=x->children[1];
                curr_node->children[2]=x->children[2];
            }
        }

        if(curr_node->node_name=="WHILE" || curr_node->node_name=="IF-THEN" || curr_node->node_name=="IF-THEN-ELSE" || curr_node->node_name=="FUNCTION") {
            sym_table.pop_back();
        }
    }

    void type_check(node *curr_node, string curr_function="") {
        if(curr_node->node_name == "FUNCTION") {
            if(curr_node->children[0]!=NULL) {
                curr_function=curr_node->children[0]->node_val;
            }
        } else if(curr_node->node_name == "MAIN") {
            while(!missing_returns.empty()) {
                cout<<"Missing return for function at line number: "<<missing_returns.begin()->second<<endl;
                missing_returns.erase(missing_returns.begin());
            }
        } else if(curr_node->node_name == "RETURN") {
            if(missing_returns.find(curr_function)!=missing_returns.end()) {
                missing_returns.erase(missing_returns.find(curr_function));
            }/* else {
                cout<<"Unexpected return at line: "<<curr_node->line_no<<endl;
            }*/
        }

        for(int i=0; i<3; i++) {
            if(curr_node->children[i]!=NULL) {
                type_check(curr_node->children[i], curr_function);
            }
        }

        if(curr_node->node_name=="INT") {
             curr_node->is_int=true;
        } else if(curr_node->node_name=="BOOL") {
            curr_node->is_bool=true;
        } else if(curr_node->node_name=="ID") {
            if(curr_node->node_val[0]=='i') {
                curr_node->is_int=true;
            } else if(curr_node->node_val[0]=='b') {
                curr_node->is_bool=true;
            }
        } else if(curr_node->node_name=="FUNCTION CALL" || curr_node->node_name == "PARENTHESIS" || curr_node->node_name == "UNARY NEGATIVE" || curr_node->node_name == "BITWISE NOT"  || curr_node->node_name == "TERM" || curr_node->node_name == "RETURN") {
            if(curr_node->children[0]!=NULL) {
                curr_node->is_bool = curr_node->children[0]->is_bool;
                curr_node->is_int = curr_node->children[0]->is_int;
            }
        } /*else if() {

        } */else if(curr_node->node_name=="BITWISE AND"||curr_node->node_name=="BITWISE OR"||curr_node->node_name=="ADD"||curr_node->node_name=="SUBTRACT"||curr_node->node_name=="MULTIPLY"||curr_node->node_name=="DIVIDE"||curr_node->node_name=="MODULO") {
            if(curr_node->children[0]!=NULL && curr_node->children[1]!=NULL) {

                if(curr_node->children[0]->is_bool!=curr_node->children[1]->is_bool || curr_node->children[0]->is_int!=curr_node->children[1]->is_int) {
                    if(type_mismatch_lines.find(curr_node->line_no)==type_mismatch_lines.end()) {
                        cout<<"Type mismatch: at line number "<<curr_node->line_no<<endl;
                        type_mismatch_lines.insert(curr_node->line_no);
                    }
                }

                curr_node->is_bool = curr_node->children[0]->is_bool && curr_node->children[1]->is_bool;
                curr_node->is_int = curr_node->children[0]->is_int && curr_node->children[1]->is_int;
            }
        } else if(curr_node->node_name=="FUNCTION CALL") {
            if(curr_node->children[0]!=NULL) {
                string fname = curr_node->children[0]->node_val;

                int i=0;
                int param_cnt=0;
                node *x = curr_node->children[1];

                while(x!=NULL and x->children[0]!=NULL) {
                    node *y = x->children[0];
                    if(y->is_int != func_params[fname][i] or y->is_bool == func_params[fname][i]) {
                        if(type_mismatch_lines.find(curr_node->line_no)==type_mismatch_lines.end()) {
                            cout<<"Type mismatch in function call: at line number "<<curr_node->line_no<<endl;
                            type_mismatch_lines.insert(curr_node->line_no);
                        }
                    }

                    x = x->children[1];
                    i++;
                    param_cnt++;
                }

                if(param_cnt!=func_params[fname].size()) {
                    cout<<"Expected "<<func_params[fname].size()<<" parameters, found "<<param_cnt<<" parameters at line: "<<curr_node->line_no<<endl;
                }
            }
        }

        if(curr_node->node_name=="RETURN") {
            if(curr_function=="" || curr_function[0]=='v') {
                cout<<"Unexpected return or return type at line: " << curr_node->line_no<<endl;
            } else if(curr_function[0]=='i' and (!curr_node->is_int)) {
                cout<<"Return type mismatch at line: " << curr_node->line_no<<endl;
            } else if(curr_function[0]=='b' and (!curr_node->is_bool)) {
                cout<<"Return type mismatch at line: " << curr_node->line_no<<endl;
            }
        }
    }

    void gen_cond(node *curr_node, int temp_no, bool func_mode) {
        node *x=curr_node;
        if(x->node_name=="COMPARE") {
            if(x->children[0]!=NULL) {
                gen_exp(x->children[0],temp_no+1,func_mode);
            }
            if(x->children[2]!=NULL) {
                gen_exp(x->children[2],temp_no+2,func_mode);
            }
            if(x->children[1]!=NULL) {
                if(x->children[1]->node_name=="EQUAL") {
                    mips<<"\tseq $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
                if(x->children[1]->node_name=="NOT EQUAL") {
                    mips<<"\tsne $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
                if(x->children[1]->node_name=="GREATER EQUAL") {
                    mips<<"\tsge $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
                if(x->children[1]->node_name=="GREATER") {
                    mips<<"\tsgt $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
                if(x->children[1]->node_name=="LESS EQUAL") {
                    mips<<"\tsle $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
                if(x->children[1]->node_name=="LESS") {
                    mips<<"\tslt $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
                }
            }
        }
        if(x->node_name=="EXPRESSION") {
            if(x->children[0]!=NULL) {
                gen_exp(x->children[0],temp_no,func_mode);
                //mips<<"\tli $t"<<temp_no+1<<",0"<<endl;
                //mips<<"\tseq $t"<<temp_no<<",$t"<<temp_no+1<<",label"<<label_cnt<<endl;
            }
        }
        if(x->node_name=="LOGICAL AND") {
            if(x->children[0]!=NULL) {
                gen_cond(x->children[0],temp_no+1,func_mode);
            }
            if(x->children[1]!=NULL) {
                gen_cond(x->children[1],temp_no+2,func_mode);
            }
            mips<<"\tand $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
        }
    }

    void gen_exp(node *curr_node, int temp_no, bool func_mode=false) {
        node *x,*y;
        ///recurse except term
        if(curr_node->node_name!="TERM") {
            for(int i=0; i<3; i++) {
                if(curr_node->children[i]!=NULL) {
                    gen_exp(curr_node->children[i],temp_no+i+1,func_mode);
                }
            }
        }
        ///term
        x=curr_node->children[0];
        if(curr_node->node_name=="TERM" && x!=NULL) {
            ///var
            if(x->node_name=="ID") {
                if(func_mode) {
                    if(func_arg_map.find(x->node_val)==func_arg_map.end()) {
                        mips<<"\tlw $t"<<temp_no<<","<<x->node_val<<endl;
                    }
                    else {
                        mips<<"\tmove $t"<<temp_no<<",$a"<<func_arg_map[x->node_val]<<endl;
                    }
                }
                else {
                    mips<<"\tlw $t"<<temp_no<<","<<x->node_val<<endl;
                }
            }
            ///int
            if(x->node_name=="INT") {
                mips<<"\tli $t"<<temp_no<<","<<x->node_val<<endl;
            }
            ///bool
            if(x->node_name=="BOOL") {
                if(x->node_val=="true") {
                    mips<<"\tli $t"<<temp_no<<",1"<<endl;
                }
                else {
                    mips<<"\tli $t"<<temp_no<<",0"<<endl;
                }
            }
            ///func call
            if(x->node_name=="FUNCTION CALL") {
                y=x;
                tmp_cnt=temp_no;
                x=x->children[1];
                while(x!=NULL) {
                    if(x->children[0]!=NULL) {
                        gen_exp(x->children[0],tmp_cnt,func_mode);
                        mips<<"\tmove $a"<<tmp_cnt-temp_no<<",$t"<<tmp_cnt<<endl;
                        tmp_cnt++;
                    }
                    x=x->children[1];
                }
                mips<<"\tjal "<<y->children[0]->node_val<<endl;
                mips<<"\tmove $t"<<temp_no<<",$v0"<<endl;
                ///restore $a0
                if(func_mode) {
                    for(int ii=3;ii>=0;ii--) {
                        mips<<"\tlw $a"<<ii<<","<<(3-ii)*4<<"($sp)"<<endl;
                    }
                }
            }
        }

        ///add
        if(curr_node->node_name=="ADD") {
            mips<<"\tadd $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
        }
        ///sub
        if(curr_node->node_name=="SUBTRACT") {
            mips<<"\tsub $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
        }
        ///mult
        if(curr_node->node_name=="MULTIPLY") {
            mips<<"\tmult $t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
            mips<<"\tmflo $t"<<temp_no<<endl;
        }
        ///div
        if(curr_node->node_name=="DIVIDE") {
            mips<<"\tdiv $t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
            mips<<"\tmflo $t"<<temp_no<<endl;
        }
        ///mod
        if(curr_node->node_name=="MODULO") {
            mips<<"\tdiv $t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
            mips<<"\tmfhi $t"<<temp_no<<endl;
        }
        ///unary
        if(curr_node->node_name=="UNARY NEGATIVE") {
            mips<<"\tli $t"<<temp_no+2<<",0"<<endl;
            mips<<"\tsub $t"<<temp_no<<",$t"<<temp_no+2<<",$t"<<temp_no+1<<endl;
        }
        ///bitwise and
        if(curr_node->node_name=="BITWISE AND") {
            mips<<"\tand $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
        }
        ///bitwise or
        if(curr_node->node_name=="BITWISE OR") {
            mips<<"\tor $t"<<temp_no<<",$t"<<temp_no+1<<",$t"<<temp_no+2<<endl;
        }
        ///parenthesis
        if(curr_node->node_name=="PARENTHESIS") {
            gen_exp(curr_node->children[0],temp_no,func_mode);
        }
    }

    void gen_mips_code(node *curr_node, bool assn=false, int temp_no=0, bool func_mode=false) {
        node *x;
        if(curr_node->node_name=="ASSIGNMENT") {
            if(curr_node->children[1]!=NULL) {
                gen_exp(curr_node->children[1],temp_no,func_mode);
            }
            if(curr_node->children[0]!=NULL) {
                if(func_mode) {
                    if(func_arg_map.find(curr_node->children[0]->node_val)==func_arg_map.end()) {
                        mips<<"\tsw $t"<<temp_no<<","<<curr_node->children[0]->node_val<<endl;
                    }
                    else {
                        mips<<"\tmove $a"<<func_arg_map[curr_node->children[0]->node_val]<<",$t"<<temp_no<<endl;
                    }
                }
                else {
                    mips<<"\tsw $t"<<temp_no<<","<<curr_node->children[0]->node_val<<endl;
                }
            }
        }
        else if(curr_node->node_name=="DECLARE") {
            if(curr_node->children[0]!=NULL) {
                gen_exp(curr_node->children[0],temp_no,func_mode);
                mips<<"\tsw $t"<<temp_no<<","<<curr_node->node_val<<endl;
            }
        }
        else if(curr_node->node_name=="INPUT") {
            if(curr_node->children[0]!=NULL) {
                mips<<"\tli $v0,5"<<endl;
                mips<<"\tsyscall"<<endl;
                mips<<"\tsw $v0,"<<curr_node->children[0]->node_val<<endl;
            }
        }
        else if(curr_node->node_name=="OUTPUT") {
            if(curr_node->children[0]!=NULL) {
                if(func_mode) {
                    ///backup $a0
                    mips<<"\taddi $sp,$sp,-4"<<endl;
                    mips<<"\tsw $a0,0($sp)"<<endl;
                }
                if(curr_node->children[0]->node_name=="STRING") {
                    mips<<"\tli $v0,4"<<endl;
                    mips<<"\tla $a0,"<<curr_node->children[0]->node_val<<endl;
                    mips<<"\tsyscall"<<endl;
                }
                else {
                    gen_exp(curr_node->children[0],temp_no,func_mode);
                    mips<<"\tli $v0,1"<<endl;
                    mips<<"\tmove $a0,$t"<<temp_no<<endl;
                    mips<<"\tsyscall"<<endl;
                    /**endline
                    mips<<"\tli $v0,4"<<endl;       ///to be commented later
                    mips<<"\tla $a0,endl"<<endl;    ///to be commented later
                    mips<<"\tsyscall"<<endl;        ///to be commented later
                    */
                }
                if(func_mode) {
                    ///restore $a0
                    mips<<"\tlw $a0,0($sp)"<<endl;
                    mips<<"\taddi $sp,$sp,4"<<endl;
                }
            }
        }
        else if(curr_node->node_name=="IF-THEN-ELSE") {
            ///if
            x=curr_node->children[0];
            if(x!=NULL) {
                gen_cond(x,temp_no,func_mode);
                mips<<"\tli $t"<<temp_no+1<<",1"<<endl;
                mips<<"\tbne $t"<<temp_no<<",$t"<<temp_no+1<<",label"<<label_cnt<<endl;
            }
            ///then
            int old_label=label_cnt++;label_cnt++;
            x=curr_node->children[1];
            if(x!=NULL) {
                gen_mips_code(x,false,temp_no,func_mode);
                mips<<"\tb label"<<old_label+1<<endl;
            }
            ///else
            x=curr_node->children[2];
            mips<<"label"+int_to_str(old_label)+":"<<endl;
            if(x!=NULL) {
                gen_mips_code(x,false,temp_no,func_mode);
            }
            mips<<"label"+int_to_str(old_label+1)+":"<<endl;
        }
        else if(curr_node->node_name=="WHILE") {
            ///while
            x=curr_node->children[0];
            int saved_val = curr_loop_label;
            curr_loop_label=label_cnt;
            mips<<"label"+int_to_str(label_cnt++)+":"<<endl;
            if(x!=NULL) {
                gen_cond(x,temp_no,func_mode);
                mips<<"\tli $t"<<temp_no+1<<",1"<<endl;
                mips<<"\tbne $t"<<temp_no<<",$t"<<temp_no+1<<",label"<<label_cnt<<endl;
            }
            ///do
            int old_label=label_cnt++;
            x=curr_node->children[1];
            if(x!=NULL) {
                gen_mips_code(x,false,temp_no,func_mode);
                mips<<"\tb label"<<old_label-1<<endl;
            }
            mips<<"label"+int_to_str(old_label)+":"<<endl;
            curr_loop_label=saved_val;
        }
        else if(curr_node->node_name=="BREAK"||curr_node->node_name=="CONTINUE") {
            if(curr_node->node_name=="BREAK") {
                if(curr_loop_label==-1) {
                    cout<<"unexpected break at line number: "<<curr_node->line_no<<endl;
                }
                else {
                    mips<<"\tb label"<<curr_loop_label+1<<endl;
                }
            }
            if(curr_node->node_name=="CONTINUE") {
                if(curr_loop_label==-1) {
                    cout<<"unexpected continue at line number: "<<curr_node->line_no<<endl;
                }
                else {
                    mips<<"\tb label"<<curr_loop_label<<endl;
                }
            }
        }
        else if(curr_node->node_name=="MAIN") {
            mips<<"main:"<<endl;
            for(int i=0; i<3; i++) {
                if(curr_node->children[i]!=NULL) {
                    gen_mips_code(curr_node->children[i]);
                }
            }
        }
        else if(curr_node->node_name=="FUNCTION") {
            x=curr_node->children[0];
            tmp_cnt=0;
            if(x!=NULL) {
                mips<<x->node_val<<":"<<endl;
                /*x=x->children[0];
                while(x!=NULL) {
                    if(x->children[0]!=NULL) {
                        mips<<"\tsw $a"<<tmp_cnt++<<","<<x->children[0]->node_val<<endl;
                    }
                    x=x->children[1];
                }*/
            }
            x=curr_node->children[1];
            if(x!=NULL) {
                ///backup
                mips<<"\taddi $sp,$sp,-4"<<endl;
                mips<<"\tsw $ra,0($sp)"<<endl;
                for(int ii=0;ii<4;ii++) {
                    mips<<"\taddi $sp,$sp,-4"<<endl;
                    mips<<"\tsw $a"<<ii<<",0($sp)"<<endl;
                }
                /**for(int ii=0;ii<10;ii++) {
                    mips<<"\taddi $sp,$sp,-4"<<endl;
                    mips<<"\tsw $t"<<ii<<",0($sp)"<<endl;
                }*/
                gen_mips_code(x,assn,temp_no,true);
                if(curr_node->children[0]->node_val[0]=='v') {
                    for(int ii=3;ii>=0;ii--) {
                        mips<<"\tlw $a"<<ii<<","<<(3-ii)*4<<"($sp)"<<endl;
                    }
                    mips<<"\taddi $sp,$sp,"<<(4)*4<<endl;
                    mips<<"\tlw $ra,0($sp)"<<endl;
                    mips<<"\taddi $sp,$sp,4"<<endl;
                    mips<<"\tjr $ra"<<endl;
                }
            }
        }
        else if(curr_node->node_name=="FUNCTION CALL") {
            tmp_cnt=temp_no;
            x=curr_node->children[1];
            while(x!=NULL) {
                if(x->children[0]!=NULL) {
                    gen_exp(x->children[0],tmp_cnt,func_mode);
                    mips<<"\tmove $a"<<tmp_cnt-temp_no<<",$t"<<tmp_cnt<<endl;
                    tmp_cnt++;
                }
                x=x->children[1];
            }
            mips<<"\tjal "<<curr_node->children[0]->node_val<<endl;
            mips<<"\tmove $t"<<temp_no<<",$v0"<<endl;
            ///restore $a0
            if(func_mode) {
                for(int ii=3;ii>=0;ii--) {
                    mips<<"\tlw $a"<<ii<<","<<(3-ii)*4<<"($sp)"<<endl;
                }
            }
        }
        else if(func_mode && curr_node->node_name=="RETURN") {
            if(curr_node->children[0]!=NULL) {
                gen_exp(curr_node->children[0],temp_no,func_mode);
                ///restore
                /**for(int ii=9;ii>=0;ii--) {
                    mips<<"\tlw $t"<<ii<<",0($sp)"<<endl;
                    mips<<"\taddi $sp,$sp,4"<<endl;
                }*/
                mips<<"\taddi $sp,$sp,"<<(4)*4<<endl;
                /*for(int ii=3;ii>=0;ii--) {
                    mips<<"\taddi $sp,$sp,4"<<endl;
                }*/
                mips<<"\tlw $ra,0($sp)"<<endl;
                mips<<"\taddi $sp,$sp,4"<<endl;

                mips<<"\tmove $v0,$t"<<temp_no<<endl;
                mips<<"\tjr $ra"<<endl;
            }
        }
        else {
            for(int i=0; i<3; i++) {
                if(curr_node->children[i]!=NULL) {
                    gen_mips_code(curr_node->children[i],false,0,func_mode);
                }
            }
        }
    }

    void generate_mips_file(node *curr_node) {
        mips<<".data"<<endl;
        for(int i=0;i<int_cnt;i++) {
            mips<<"\ti"<<i<<": .word 0"<<endl;
        }
        for(int i=0;i<bool_cnt;i++) {
            mips<<"\tb"<<i<<": .word 0"<<endl;
        }
        for(int i=0;i<str_cnt;i++) {
            mips<<"\ts"<<i<<": .asciiz "<<str_map["s"+int_to_str(i)]<<endl;
        }
        mips<<"\tendl: .asciiz \"\\n\""<<endl;
        mips<<".text"<<endl;
        gen_mips_code(curr_node);
        mips<<"\tli $v0,10"<<endl;
        mips<<"\tsyscall"<<endl;
    }


    void stupid_print(node *curr_node) {
        if(curr_node==NULL)
            return;

        cout<<curr_node->node_name<<" "<<curr_node->node_val<<endl;

        for(int i=0; i<3; i++) {
            stupid_print(curr_node->children[i]);
        }
    }

    void better_print(node *curr_node, vector<bool> f, int depth=0) {
        if(curr_node==NULL) return;

        for(int i=0; i<depth-1; i++) {
            if(f[i]) f_tree<<"|"; else f_tree<<" ";
            f_tree<<"       ";
        }
        for(int i=depth-1; i<depth and i>=0; i++) {
            f_tree<<"|-------";
        }

        f_tree<<curr_node->node_name<<" "<<curr_node->node_val<<endl;

        for(int i=0; i<3; i++) {
            if(depth==f.size()) f.push_back(true);

            if(i<2 and curr_node->children[i+1]!=NULL) f[depth]=true;
            else    f[depth]=false;

            better_print(curr_node->children[i], f, depth+1);
        }
    }


} ;
