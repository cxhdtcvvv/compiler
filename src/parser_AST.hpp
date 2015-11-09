#ifndef parser_AST_hpp
#define parser_AST_hpp

#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>


extern std::vector<std::string> id_stack;

/********************BASIC BLOCKS*****************/

class VALUE
{
public:
    virtual ~VALUE()
    {}
    virtual void Print(std::ostream &dst) const=0;
    virtual int node_id(){return -1;}
};


class INTEGER : public VALUE
{
private:
    int value;
    
public:
    INTEGER(int i):value(i){}
    
    virtual void Print(std::ostream &dst) const
    {
        dst<<value;
    }
    
};


class DOUBLE: public VALUE
{
private:
    double value;
    
public:
    DOUBLE(double i):value(i){}
    
    virtual void Print(std::ostream &dst) const
    {
        dst<<value;
    }
    
};


class ID: public VALUE
{
private:
    std::string value;
    
public:
    ID(std::string i):value(i){}
    
    std::string getid(){return value;}
    
    virtual void Print(std::ostream &dst) const
    {
        dst<<value;
    }
    
};

class OPERATOR: public VALUE
{
private:
    char value;
    
public:
    OPERATOR(char i):value(i){}
    
    virtual void Print(std::ostream &dst) const
    {
        dst<<value;
    }
    
};

class TYPE: public VALUE
{
private:
    std::string value;
    
public:
    TYPE(std::string i):value(i){}
    
    virtual void Print(std::ostream &dst) const
    {
        dst<<value;
    }
    
};


class FACTOR: public VALUE
{
    
public:
    int type;
    INTEGER* f_int;     //1
    DOUBLE* f_double;   //2
    ID* f_id;           //3
    FACTOR(){}
    
    virtual void Print(std::ostream &dst) const
    {
        
        switch (type)
        {
            case 1:
                f_int->Print(dst);
                break;
            case 2:
                f_double->Print(dst);
                break;
            case 3:
                f_id->Print(dst);
                break;
            default:
                break;
        }
    }
    
};


class EXPR: public VALUE
{
    
public:
    int type;
    int f_int;     //1
    double f_double;   //2
    std::string f_id;     //3
    char f_op;          //5
    std::string f_string;  //6
    std::vector<EXPR*> expr_3;         //4
    
    
    EXPR(){}
    
    void Add(EXPR* i1,EXPR* i2,EXPR* i3)
    {
        expr_3.push_back(i1);
        expr_3.push_back(i2);
        expr_3.push_back(i3);
    }
    
    
    void debug(std::map<std::string, std::string > id_map){
        if(type == 3){
            if( id_map.find(f_id)==id_map.end() )
            {
                std::cout<<"***error:variable used is not declared"<<std::endl;
                exit(1);
            }
        }
        else if(type == 4)
        {
            expr_3[0]->debug(id_map);
            expr_3[2]->debug(id_map);
        }
    }
    
    int find_id(std::string _id) const
    {
        for(unsigned i=0; i<id_stack.size(); i++)
        {
            if(id_stack[i] == _id)
            {
                return i;
            }
        }
        return -1;
    }
    
    
    virtual void Print(std::ostream &dst) const{
        if(type == 4){
            Print_expr(dst,0);
        }else{
            Print_single(dst);
        }
    }
    
    void operating(std::ostream &dst) const{
        if(expr_3[1]->f_op == '+')
        {
            dst<<"add r1, r0, r1"<<std::endl;
        }
        else if(expr_3[1]->f_op == '*')
        {
            dst<<"mul r1, r0, r1"<<std::endl;
        }else if(expr_3[1]->f_op == '-')
        {
            dst<<"sub r1, r0, r1"<<std::endl;
        }else if(expr_3[1]->f_op == '/')
        {
            dst<<"udiv r1, r0, r1"<<std::endl;
        }
    }
    
    virtual void Print_expr(std::ostream &dst, int flag) const
    {
        switch (type) {
                
            case 1:
                dst<<f_int;
                break;
            case 2:
                dst<<f_double;
                break;
            case 3:
                if(flag == 1){          //second oprand
                    int stack_addr=find_id(f_id)+1;
                    dst<<"ldm r1, [fp, #-"<< 4+stack_addr*4  <<"]"<<std::endl;
                }else if(flag == 0){    //first oprand
                    int stack_addr=find_id(f_id)+1;
                    dst<<"ldm r0, [fp, #-"<< 4+stack_addr*4  <<"]"<<std::endl;
                }
                break;
            case 5:
                dst<<f_op;
                break;
            case 6:
                dst<<f_string;
                break;
            case 4:             //START!!!
                
                if(expr_3[0]->type == 4)
                {
                    
                    expr_3[0]->Print_expr(dst,0);
                    dst<<"mov r3, r1"<<std::endl;
                    
                    if(expr_3[2]->type != 4)
                    {
                        if(expr_3[2]->type == 3){

                            expr_3[2]->Print_expr(dst,1);
                        }else{
                            dst<<"mov r1, #";
                            expr_3[2]->Print_expr(dst,0);
                            dst<<std::endl;
                        }
                    }else   //then r0 could not be used!!!!!
                    {
                        expr_3[2]->Print_expr(dst,0);
                    }
                    
                        //.....operation
                    if(expr_3[1]->f_op == '+')
                    {
                        dst<<"add r1, r3, r1"<<std::endl;
                    }
                    else if(expr_3[1]->f_op == '*')
                    {
                        dst<<"mul r1, r3, r1"<<std::endl;
                    }else if(expr_3[1]->f_op == '-')
                    {
                        dst<<"sub r1, r3, r1"<<std::endl;
                    }else if(expr_3[1]->f_op == '/')
                    {
                        dst<<"udiv r1, r3, r1"<<std::endl;
                    }
                    
                }
                else{
                    
                    if(expr_3[2]->type == 4)
                    {
                        expr_3[2]->Print_expr(dst,0);
                        
                        if(expr_3[0]->type == 3){
                            expr_3[0]->Print_expr(dst,0);
                        }else{
                            dst<<"mov r0, #";
                            expr_3[0]->Print_expr(dst,0);
                            dst<<std::endl;
                        }
                        
                        operating(dst);
                    }
                    else
                    {
                        if(expr_3[0]->type == 3){
                            expr_3[0]->Print_expr(dst,0);
                        }else{
                            dst<<"mov r0, #";
                            expr_3[0]->Print_expr(dst,0);
                            dst<<std::endl;
                        }
                        
                        if(expr_3[2]->type == 3){
                            expr_3[2]->Print_expr(dst,1);
                        }else{
                            
                            dst<<"mov r1, #";
                            expr_3[2]->Print_expr(dst,0);
                            dst<<std::endl;
                        }
                        operating(dst);
                    }
                }
                break;
            default:
                break;
        }

    }
    
    
    
    virtual void Print_single(std::ostream &dst) const
    {
            switch (type)
            {
                case 1:
                    dst<<"mov r1, #"<<f_int<<std::endl;
                    break;
                case 2:
                    dst<<"mov r1, #"<<f_double<<std::endl;
                    break;
                case 3:
                    int stack_addr=find_id(f_id)+1;
                    dst<<"ldm r1, [fp, #-"<< 4+stack_addr*4 <<"]"<<std::endl;
                    break;
              

            
            }
    }
    
    
    
    
    
};





class FUNC_INPUT: public VALUE
{
private:
    std::vector<EXPR*> entries;

public:
    FUNC_INPUT() {}
    
    void Add(EXPR *expr)
    {
        entries.push_back(expr);
    }
    
    virtual void Print(std::ostream &dst) const
    {
        for(unsigned i=0; i<entries.size(); i++){
            dst<<"func variable: "; entries[i]->Print(dst); dst<<std::endl;
        }
    }


};


class IDENS_v: public VALUE
{
	std::vector<std::string> entries; 
public:
	IDENS_v(){}
	void Add ( std::string a){
	    entries.push_back(a);
	}
    
	std::vector<std::string> get_vector(){return entries;}
    
    virtual void Print(std::ostream &dst) const{dst<<std::endl;}
};




/********************NODE*****************/



class ASSIGNMENT: public VALUE
{
private:
    std::string name;
    EXPR* expr;
    

public:	
    ASSIGNMENT( std::string Name, EXPR* Value): name(Name), expr(Value)
    {}
    
    std::string get_id(){return name;}
    EXPR* get_expr(){return expr;}
    int find_id(std::string _id) const
    {
        for(unsigned i=0; i<id_stack.size(); i++)
        {
            if(id_stack[i] == _id)
            {
                return i;
            }
        }
        return -1;
    }
    
    int node_id(){return 101;}
    
    virtual void Print(std::ostream &dst) const
    {
        
        
        int stack_addr=find_id(name)+1;
        expr->Print(dst);
        
        dst<<"mov r3, r1"<<std::endl;            //TO_DO:put register to stack
        dst<<"str r3, [fp, #-"<< 4+stack_addr*4  <<"]";
        
        
    }
};

class FUNC_CALL: public VALUE
{
private:
    std::string fname;
    FUNC_INPUT* f_input;
public:	
    FUNC_CALL( std::string Name, FUNC_INPUT* F_input): fname(Name), f_input(F_input)
    {}

    int node_id(){return 102;}

    virtual void Print(std::ostream &dst) const
    {
	dst<<"function call: "<<fname;
	dst<<std::endl;
	f_input->Print(dst);
    }

};

class DECLARATION: public VALUE
{
private:
    int property;	
    std::string type;
    std::string name;
    EXPR* expr;
    std::vector<std::string> id_vector;
    
public:	
    DECLARATION(){}

    int getproperty(){return property;}
    std::string gettype(){return type;}
    std::string getid(){return name;}
    EXPR* getexpr(){return expr;}
    std::vector<std::string> getids()  {return id_vector; }
    

    int find_id(std::string _id) const
    {
        for(unsigned i=0; i<id_stack.size(); i++)
        {
            if(id_stack[i] == _id)
            {
                return i;
            }
        }
        return -1;
    }

    
    void Add_1 ( std::string Type, std::string Name, EXPR* Value, int p){   //property 1: int a =3;
	type=Type;  name = Name;   expr = Value;  property= p;
    }

    void Add_2 (std::string Type, IDENS_v* ids,int p){			//property 2: int a,b,c;
	property= p;
	type =Type;
	id_vector=ids->get_vector();
    }	


    virtual void Print(std::ostream &dst) const
    {
        if(property == 1)
        {
            
            expr->Print(dst);       //has calculated the value
            dst<<"mov r3, r1"<<std::endl;            //TO_DO:put register to stack
            id_stack.push_back(name);
            int stack_addr = find_id(name)+1;
            dst<<"str r3, [fp, #-"<< 4+stack_addr*4  <<"]";
            
        }
        
        else if(property == 2)
        {
            for(unsigned i=0; i<id_vector.size(); i++)
            {
                if(i!=0)
                    dst<<std::endl;
                dst<<"mov r3, 0"<<std::endl;
                id_stack.push_back(id_vector[i]);
                int stack_addr = find_id(id_vector[i])+1;
                dst<<"str r3, [fp, #-"<< 4+stack_addr*4  <<"]";
                

            }
        }
    }
};


class FUNC_UNIT:public VALUE
{
    
public:
    int type;
    DECLARATION* decla;     //1
    ASSIGNMENT* assign;     //2
    FUNC_CALL* funcall;     //3
    
    FUNC_UNIT(){}
    virtual void Print(std::ostream &dst) const{
        
        switch (type) {
            case 1:
                decla->Print(dst);
                break;
            case 2:
                assign->Print(dst);
                break;
            case 3:
                funcall->Print(dst);
                break;
            default:
                break;
        }
    }
};



/********************ROOT*****************/
class FUNC_IMPL: public VALUE
{
    
private:
    std::vector<FUNC_UNIT*> entries;
    
    typedef std::map<std::string ,std::string> map_type;
    std::map<std::string, std::string > id_map;

public:
    FUNC_IMPL(){}
    
    void Add(FUNC_UNIT* i){
        entries.push_back(i);
    }

    virtual void Print(std::ostream &dst) const
    {
        //intialise STACK
        dst<<"str	fp, [sp, #-4]!"<<std::endl;
        dst<<"add	fp, sp, #0"<<std::endl;
        if(id_map.size()%2)
        {
            dst<<"sub	sp, sp, #"<<4+id_map.size()*4+4<<std::endl;
        }else
        {
            dst<<"sub	sp, sp, #"<<4+id_map.size()*4<<std::endl;
        }
        
        //*******
        for(unsigned i=0; i<entries.size(); i++){
            if(i!=0)
                dst<<std::endl;
            entries[i]->Print(dst);
        }
    }
    
    void build_map(){
        if((entries.back())->type == 1){
            DECLARATION* temp = entries.back()->decla;
            if(temp->getproperty() == 1){
                id_map[temp->getid()] = temp->gettype();
                
                std::cout<<"build map:"<<std::endl;
                std::cout<<"map：  "<<id_map.size()<<std::endl;
                map_type::iterator it=id_map.begin();
                for(unsigned i=0; i<id_map.size(); i++){
                    std::cout<<it->first <<"->"<<it->second<<std::endl;
                    it++;
                }

            }else if(temp->getproperty() == 2){
                std::vector<std::string> temp_ids  = temp->getids();
                std::string temp_type = temp->gettype();
                
                for(unsigned i=0; i<temp_ids.size(); i++){
                    id_map[temp_ids[i]] = temp_type;
                    }
                
                std::cout<<"build map:"<<std::endl;
                std::cout<<"map：  "<<id_map.size()<<std::endl;
                map_type::iterator it=id_map.begin();
                for(unsigned i=0; i<id_map.size(); i++){
                    std::cout<<it->first <<"->"<<it->second<<std::endl;
                    it++;
                }
                
            }
        }
   }
    
    
    
    
    
    void debug(FUNC_UNIT* i){
        debug_decla_LHS(i);
        debug_decla_RHS(i);
        debug_assign(i);
    }
    

    
    void debug_assign(FUNC_UNIT* i){
        
        if(i->type == 2)
        {       //assignment
            ASSIGNMENT* temp_assign = i->assign;
            if( id_map.find(temp_assign->get_id()) == id_map.end())
            {
                std::cout<<"***error:variable doesn't exists"<<std::endl;
                exit(1);
            }
            
            (temp_assign->get_expr())->debug(id_map);

        }

    }
    
    
    
    
    void debug_decla_LHS(FUNC_UNIT* i){
        if(i->type == 1){
            if((i->decla)->getproperty() == 1){        //int a=4;  int a = a+b;
                //check LHS
                std::string temp_id = (i->decla)->getid();
                if( id_map.find(temp_id) != id_map.end() ){
                    std::cout<<"***error:variable already exists, redeclare"<<std::endl;
                    exit(1);
                }

            }else{  //int a,b,c;
                std::vector<std::string> temp_ids =(i->decla)->getids();
                for(unsigned i=0; i<temp_ids.size(); i++){
                    for(unsigned j=i+1; j<temp_ids.size(); j++){
                        if(temp_ids[i]==temp_ids[j]){
                            std::cout<<"***error: same variables in declaration"<<std::endl;
                            exit(1);
                        }
                    }
                }

                for(unsigned j=0; j<temp_ids.size(); j++){
                    if(id_map.find(temp_ids[j]) != id_map.end()){
                        std::cout<<"***error: variable already exists, redeclare"<<std::endl;
                        exit(1);
                    }
                }
            }

        }
    }
    
    void debug_decla_RHS(FUNC_UNIT* i){
        if(i->type == 1)
        {   //it's decla
            if((i->decla)->getproperty() == 1)
            {
                DECLARATION* temp_decla = (i->decla);
                (temp_decla->getexpr())->debug(id_map);
                
            }
        }

    }

    

//DECLARATION* decla;     //1
//ASSIGNMENT* assign;     //2
//FUNC_CALL* funcall;     //3
//
};

class PROGRAM: public VALUE
{
public:
    FUNC_IMPL* func;

};







#endif
