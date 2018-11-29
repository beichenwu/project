#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <cstdlib>
#include <minisat/core/Solver.h>
#include "minisat/core/SolverTypes.h"
#include "vsover.h"
#include <algorithm>
#include <set>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


using namespace std;

vector<vector<int> > Create_Node_list(int NumberofVertices, vector<vector<int> >& Edge_List){  //Define an array to store all node information
    vector<vector<int> > Vertex_Vector;
    for(int i = 0; i < NumberofVertices; i++){  //For Loop to create an array has size equal to number of Vertices
        vector<int> Vertex;                    //Set vector for each vertex
        Vertex.push_back(i);                   //Set the name for each Vertex
        Vertex.push_back(-1);                   //Set the parts for 0(For None)
        Vertex.push_back(0);                   //Set the Node Status(0 for searched and 1 for searched)
        Vertex_Vector.push_back(Vertex);       //Create the Vector
    }
    Edge_List.clear();                         //Reset Edge List
    return  Vertex_Vector;
}

int Read_Number_Of_Vertices(string User_Input){                //Define an function to analyze the user input and return number of vertices
    int Number_Of_vertices;                                    //Create a variable to store the number of vertices
    int String_Index;                                          //a variable to find where the space from the input
    int String_Length;                                         //a variable to store the length of string
    String_Index = User_Input.find(" ");                       //Locate the position of " "
    String_Length = User_Input.length();                       //The length of the string
    istringstream(User_Input.substr(String_Index+1, String_Length-String_Index)) >> Number_Of_vertices;     //Read the string and get the number of vertex
    return  Number_Of_vertices;
}


vector<string> String_Split(string String_Input, string Deliminator){           //A Function split a string and return a vector
    int String_Index = 0;                                                       //Initialized the String Index
    vector<string> Splited_String;                                              //Initialized the Vector
    string Sub_String;                                                          //Initialized the substring
    string String_For_Split = String_Input;                                     //Record the string for split
    while(String_Index != -1){                                                  //Stop the loop if deliminator is not found
        String_Index = String_For_Split.find(Deliminator);
        Sub_String = String_For_Split.substr(0,String_Index);                   //Find the substring
        Splited_String.push_back(Sub_String);                                   //Added the substring to vector
        String_For_Split.erase(0, String_Index+1);                              //Re-size the string
    }
    return Splited_String;
}

vector<vector<int> > Read_Edge_list(string User_Input, vector<vector<int> > VerticesVector, int& error){                  //Define an function to analyze the user input and return array of edge
    int String_Index1;
    int String_Index2;                                                                                      //a variable to find where the space from the input
    int Edge_Number;                                                                                        //a variable to store the number of edge
    int Node_Length = VerticesVector.size();
    string Edge_String = User_Input;                                                                        //Local String to store the user input
    vector<string> Tmp_Edge_List = String_Split(Edge_String, "<");                                          //Store the string in a temporary vector
    Edge_Number = Tmp_Edge_List.size();                                                                     //Determine the length of the edge list
    vector<vector<int> > Edge_List;                                                                         //Initialized Pointer for 2D Edge_list
    for(int i=0; i < Edge_Number; i++){
        if(i >=1) {                                                                                        //Loop Start from 1(Since index 0 is "E {"
            vector<int> Edge_Pair;
            String_Index1 = Tmp_Edge_List[i].find(",");                                                    //Substring to find the vertex number
            String_Index2 = Tmp_Edge_List[i].find(">");
            int tmp_int;
            istringstream(Tmp_Edge_List[i].substr(0, String_Index1)) >> tmp_int;
            Edge_Pair.push_back(tmp_int);
            istringstream(Tmp_Edge_List[i].substr(String_Index1+1, (String_Index2-String_Index1-1))) >> tmp_int;
            Edge_Pair.push_back(tmp_int);
            Edge_List.push_back(Edge_Pair);                                                                //A Function is needed to validate the edge list
        }
    }
    for(int i = 0; i < Edge_List.size(); i++){                                                             //Error Check
        if(Edge_List[i][0]> (Node_Length-1) || Edge_List[i][1]> (Node_Length-1) ){
            error = error + 1;
            cerr << "Error: Edge List contains nodes beyond the node list" << endl;
            break;
        }
    }
    return Edge_List;
}

int Most_Incident_Vertex(int v_number, vector<vector<int> > e_list){
    vector<int> tmp_vecotr;
    int tmp_max_index;
    for(int i =0; i < v_number; i++){
        tmp_vecotr.push_back(0);
    }
    for(int i = 0; i < e_list.size(); i++){
        tmp_vecotr[e_list[i][0]] += 1;
        tmp_vecotr[e_list[i][1]] += 1;
    }
    auto tmp_max  = max_element(tmp_vecotr.begin(), tmp_vecotr.end());
    tmp_max_index = distance(tmp_vecotr.begin(), tmp_max);
    return tmp_max_index;
}

vector<vector<int>> Remove_Edge(int vertex, vector<vector<int> > e_list, int vertex2 = -1){
    vector<vector<int>> new_edge_list;
    if(vertex2 == -1){
        for(vector<int> e: e_list){
            if(e[0] != vertex and e[1] != vertex){
                new_edge_list.push_back(e);
            }
        }
        return new_edge_list;
    }else{
        for(vector<int> e: e_list){
            if(e[0] != vertex && e[1] != vertex && e[0] != vertex2 && e[1] != vertex2){
                new_edge_list.push_back(e);
            }
        }
        return new_edge_list;
    }
}

int index_Generator(int e_list_length){
    int index;
    FILE *fp;
    fp = fopen("/dev/urandom", "r");
    fread(&index, 1, sizeof(index), fp);
    index = index % (e_list_length);
    fclose(fp);
    index = abs(index);
    return index;
}

int Vertex_Number(vector<vector<int>> e_list){
    int tmp_max = 0;
    for (vector<int> e: e_list){
        for (int e2:e){
            if(tmp_max < e2){
                tmp_max = e2;
            }
        }
    }
    return tmp_max;
}

string Read_Solution(vector<int> sol){
    string out = "CNF-SAT-VC: ";
    for (int i = 0; i < sol.size(); i++) {
        if (sol[i] == 1) {
            out += to_string(i) + " ";
        }
    }
    out.pop_back();
    return out;
}

void *CNF_SAT_VC(void *arg){
    vector<vector<int>> *e_list = static_cast<vector<vector<int>>*>(arg);
    int number = Vertex_Number(*e_list)+1;
    int min = 1;
    int max = number;
    int mid = floor((number+1)/2);
    while(true){
        Solver s(number, mid, *e_list);
        if (s.solve()) {
            auto solution = s.get_solution();
            Solver s(number, mid-1, *e_list);
            if(!s.solve()){
                string tmp_out = Read_Solution(solution);
                cout << tmp_out << endl;
                break;
            }else{
                max = mid;
                mid = floor((max + min)/2);
            }
        }else{
            min = mid;
            mid = floor((max + min)/2);
        }
    }
}

void *APPROX_VC_1(void *arg){
    vector<vector<int>> *e_list = static_cast<vector<vector<int>>*>(arg);
    set<int> vector_cover;
    vector<vector<int>> tmp_e_list;
    copy(e_list->begin(),e_list->end(),back_inserter(tmp_e_list));
    int max_vertex;
    int v_number = Vertex_Number(*e_list)+1;
    string out_put = "APPROX_VC_1:";

    while(tmp_e_list.size() > 0){
        max_vertex = Most_Incident_Vertex(v_number, tmp_e_list);
        tmp_e_list = Remove_Edge(max_vertex, tmp_e_list);
        vector_cover.insert(max_vertex);
    }
    for(int e: vector_cover){
        out_put += " " + to_string(e);
    }
    cout <<out_put<<endl;
}

void *APPROX_VC_2(void *arg){
    vector<vector<int>> *e_list = static_cast<vector<vector<int>>*>(arg);
    set<int> vector_cover;
    vector<vector<int>> tmp_e_list;
    copy(e_list->begin(),e_list->end(),back_inserter(tmp_e_list));
    int index;
    string out_put = "APPROX_VC_2:";

    while(tmp_e_list.size() > 0){
        index = index_Generator(e_list->size());
        tmp_e_list = Remove_Edge((*e_list)[index][0], tmp_e_list, (*e_list)[index][1]);
        vector_cover.insert((*e_list)[index][0]);
        vector_cover.insert((*e_list)[index][1]);
    }
    for(int e: vector_cover){
        out_put += " " + to_string(e);
    }
    cout <<out_put<<endl;
}

static void pclock(char *msg, clockid_t cid)
{

    struct timespec ts;

    if (clock_gettime(cid, &ts) == -1)
        handle_error("clock_gettime");
    printf("%4ld.%010ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
}



int main() {
    string User_Input;                                             //Assign an empty string to read user input
    vector<vector<int> > VerticesVector;                           //Assign an pointer refer to vertices array
    vector<vector<int> > Edge_List;                                //Assign an empty vector to store the edge
    string Out_Put_Mini;
    vector<vector<string>> variable_string;
    while(true) {                                                  //Main loop of the file

        getline(cin, User_Input);                                 //Take system standard input
        if (User_Input == "") {                                      //Exit the program if user input empty string
            break;
        } else if (User_Input.substr(0, 1) == "V") {                    //For the case the user input vertices
            int Number_Of_Vertices;                                //Create a variable to store the number of vertices
            Number_Of_Vertices = Read_Number_Of_Vertices(User_Input);
            VerticesVector = Create_Node_list(Number_Of_Vertices, Edge_List);                     //Create the node list

        } else if (User_Input.substr(0, 1) == "E") {                    //For the case the user input edge set
            int Error = 0;
            Out_Put_Mini = "";
            Edge_List = Read_Edge_list(User_Input, VerticesVector, Error);
            int number = VerticesVector.size();
            //Solver
            if (Error == 0){
                clockid_t cid_CNF;
                clockid_t cid_1;
                clockid_t cid_2;

                pthread_t CNF_SAT_VC_Thread;
                pthread_t APPROX_VC_1_Thread;
                pthread_t APPROX_VC_2_Thread;


                pthread_create(&CNF_SAT_VC_Thread, NULL, CNF_SAT_VC, &Edge_List);
                pthread_getcpuclockid(CNF_SAT_VC_Thread, &cid_CNF);
                pthread_join(CNF_SAT_VC_Thread,NULL);
                pclock("CNF_SAT_VC time:", cid_CNF);

                pthread_create(&APPROX_VC_1_Thread, NULL, APPROX_VC_1, &Edge_List);
                pthread_getcpuclockid(APPROX_VC_1_Thread, &cid_1);
                pthread_join(APPROX_VC_1_Thread,NULL);

                pclock("APPROX_VC_1 time:", cid_1);

                pthread_create(&APPROX_VC_2_Thread, NULL, APPROX_VC_2, &Edge_List);
                pthread_getcpuclockid(APPROX_VC_2_Thread, &cid_2);
                pthread_join(APPROX_VC_2_Thread,NULL);
                pclock("APPROX_VC_2_Time:", cid_2);

            }
        }
    }

    return 0;
}

//for(int i = 0; i < variable_string.size();i++){
    //for (int j = 0; j < variable_string[i].size();j++){
        //cout << variable_string[i][j]<<" ";
    //}
    //cout << "0 \n";
//}