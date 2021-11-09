#include<iostream>
#include<vector>
#include<math.h>
#include <map>
#include<string>
#include<bits/stdc++.h>
using namespace std;


class DES_alg{
public:
	DES_alg(){
		hex_to_binary_map['0']="0000";binary_to_hex_map["0000"]="0";
		hex_to_binary_map['1']="0001";binary_to_hex_map["0001"]="1";
		hex_to_binary_map['2']="0010";binary_to_hex_map["0010"]="2";
		hex_to_binary_map['3']="0011";binary_to_hex_map["0011"]="3";

		hex_to_binary_map['4']="0100";binary_to_hex_map["0100"]="4";
		hex_to_binary_map['5']="0101";binary_to_hex_map["0101"]="5";
		hex_to_binary_map['6']="0110";binary_to_hex_map["0110"]="6";
		hex_to_binary_map['7']="0111";binary_to_hex_map["0111"]="7";

		hex_to_binary_map['8']="1000";binary_to_hex_map["1000"]="8";
		hex_to_binary_map['9']="1001";binary_to_hex_map["1001"]="9";
		hex_to_binary_map['A']="1010";binary_to_hex_map["1010"]="A";
		hex_to_binary_map['B']="1011";binary_to_hex_map["1011"]="B";

		hex_to_binary_map['C']="1100";binary_to_hex_map["1100"]="C";
		hex_to_binary_map['D']="1101";binary_to_hex_map["1101"]="D";
		hex_to_binary_map['E']="1110";binary_to_hex_map["1110"]="E";
		hex_to_binary_map['F']="1111";binary_to_hex_map["1111"]="F";
	}
	string hex_to_binary(string hex_data){
		string binary;
		for(int i=0;i<hex_data.length();i++){
			binary+=hex_to_binary_map[hex_data[i]];
		}
		return binary;
	}
	string binary_to_hex(string binary){
		string hex;
		for(int i = 0;i<binary.length();i+=4)
			hex+=binary_to_hex_map[binary.substr(i,4)];
		return hex;
	}
	string permute(string data,int *permutation_array,int output_length){
		string output="";
		for(int i = 0;i<output_length;i++){	
			output += data[permutation_array[i]-1];
		}
		return output;
	}
	int binary_to_int(string binary){
		int num=0;
		for(int i=0;i<binary.length();i++){
			if(binary[i] == '1')
				num+=pow(2,binary.length()-i-1);
		}
		return num;
	}
	string int_to_binary(int num){
		string binary="";
		while(num != 0){
			if(num%2)
				binary+="1";
			else
				binary +="0";
			num=(int)num/2;
		}
		reverse(binary.begin(), binary.end());
		return binary;
	}

	string s_box_func(string input_48bit){
		string output_32bit;
		string selector_2bit="";
		string selector_4bit="";
		int sel_2 = 0;
		int sel_4 = 0;
		int s_val =0;
		//cout<<"=============in s box ==================="<<endl;
		for(int i=0;i<48;i+=6){
			selector_2bit = input_48bit.substr(i,1)+input_48bit.substr(i+5,1);
			selector_4bit = input_48bit.substr(i+1,4);
			sel_2 = binary_to_int(selector_2bit);
			sel_4 = binary_to_int(selector_4bit);
			//cout<<"selectors =>> "<<i/6<<","<<sel_2<<","<<sel_4<<endl;
			s_val = this->s_boxes[i/6][sel_2][sel_4];
			//cout<<"val = "<<s_val<<endl;
			//cout<<"binary = ";
			for(int i=0;i<4-int_to_binary(s_val).length();i++){
				output_32bit+="0";
				//cout<<"0";
			}
			//cout<<int_to_binary(s_val)<<endl;
			output_32bit += int_to_binary(s_val);
		}
		//cout<<"================================"<<endl;
		return output_32bit;
	}
	string left_circular_shift(string binary,int shift_amount){
		string shifted_binary="";
		char temp;
		for(int i = 0;i<shift_amount;i++){
			shifted_binary+=binary.substr(1,binary.length()-1);
			shifted_binary+=binary[0] ;
			binary = shifted_binary;
			shifted_binary = "";
		}
		return binary;
	}
	string xor_s(string binary1 ,string binary2){
		string result = "";
		for(int i=0;i<binary1.length();i++){
			if(binary1[i]=='1' && binary2[i]=='0' ||binary1[i]=='0' && binary2[i]=='1'){
				result+="1";
			}else{
				result+="0";
			}
		}
		return result;
	}

	string DES_round(string data,string key48bit,int round_num){
		string output64bit="";
		string left_32bit = data.substr(0,32);
		string right_32bit = data.substr(32,32);
		

		string expantion_permuted48bit = permute(right_32bit,this->expansion_permutation,48);
		//cout<<"expantion_permuted48bit ==> "<<binary_to_hex(expantion_permuted48bit)<<endl;

		string data_xor_key48bit = xor_s(expantion_permuted48bit,key48bit);
		//cout<<"data_xor_key48bit ==> "<<binary_to_hex(data_xor_key48bit)<<endl;

		string s_box32_output32bit = s_box_func(data_xor_key48bit);
		//cout<<"s_box32_output32bit ==> "<<binary_to_hex(s_box32_output32bit)<<endl;

		string p_permuted32bit = permute(s_box32_output32bit,this->permutation_p,32);
		//cout<<"p_permuted32bit ==> "<<binary_to_hex(p_permuted32bit)<<endl;
		if(round_num==15){
			output64bit+=xor_s(left_32bit,p_permuted32bit);
			output64bit+=right_32bit;
		}else{
			output64bit+=right_32bit;
			output64bit+=xor_s(left_32bit,p_permuted32bit);
		}
		return output64bit;
	}

	string encrypt(string text ,string key){
		string data_64bit = hex_to_binary(text);
		string round_in64bit = permute(data_64bit,this->intial_permutation,64);

		//cout<<"after intial permutation"<<"=>>"<<binary_to_hex(round_in64bit)<<endl<<"len"<<round_in64bit.length()<<endl;

		string round_output64bit="";

		string key_64bit = hex_to_binary(key);
		string round_key56bit = permute(key_64bit,this->permuted_choice_one,56);
		string permuted_key48bit="";


		for(int round=0;round<16;round++){
			//cout<<"============= DES Round "<<round<<" ================="<<endl;
			string left_key = round_key56bit.substr(0,28);
			string right_key = round_key56bit.substr(28,28);
			round_key56bit = left_circular_shift(left_key,this->bit_rotates[round]) +  left_circular_shift(right_key,this->bit_rotates[round]);

			permuted_key48bit = permute(round_key56bit,this->permuted_choice_two,48);
			this->key48bit_vector.push_back(permuted_key48bit);
			//cout<<"PERMUTED KEY 48BIT=>"<<binary_to_hex(permuted_key48bit)<<endl;
			
			round_in64bit=DES_round(round_in64bit,permuted_key48bit,round);

			//cout<<"round output ==> "<<binary_to_hex(round_in64bit)<<endl;
					//cout<<"============================================"<<endl;
		//cout<<endl<<endl;
		}
		reverse(key48bit_vector.begin(), key48bit_vector.end());

		return binary_to_hex(permute(round_in64bit,this->inverse_intial_permutation,64));

	}
	string decrypt(string text){
		string round_in64bit = permute(hex_to_binary(text),this->intial_permutation,64);

		for(int round=0;round<16;round++){
			string round_key48bit = this->key48bit_vector[round];
			round_in64bit=DES_round(round_in64bit,round_key48bit,round);
		}
		return binary_to_hex(permute(round_in64bit,this->inverse_intial_permutation,64));

		
	}

	void test_s_box(){
		string s="0123456789ab";
		string t = hex_to_binary(s);
		//cout<<t.length()<<endl;
		//cout<<t<<endl;
	//cout<<s_box_func(t).length()<<endl;
		//cout<<left_circular_shift("0010101",3);
		string data="7A6C731D22347676";
		string key="1323445A6D788381";
		cout<<endl;
		cout<<"data"<<(data).length()<<"=>"<<""<<(data)<<endl;
		//cout<<"data"<<hex_to_binary(data).length()<<"=>"<<""<<hex_to_binary(data)<<endl;
		//cout<<"key"<<hex_to_binary(key).length()<<"=>"<<""<<hex_to_binary(key)<<endl;
		string d = (encrypt(data,key));
		cout<<"encrypted ==> "<<d<<endl;
		cout<<"decrypted ==> "<<decrypt(d)<<endl;
	}
	void test_permute(string in){
		string out;
		cout<<in<<endl;
		cout<<binary_to_hex(permute(permute(hex_to_binary(in),this->intial_permutation,64),this->inverse_intial_permutation,64));
		cout<<endl<<int_to_binary(5)<<endl;
		cout<<endl<<binary_to_int("1111")<<endl;
	}

	void test_conv(){
		string s="0123456789abcdef";
		string t = hex_to_binary(s);
		for(int i=0;i<64;i+=4)
			cout<<t.substr(i,4)<<endl;
		t = binary_to_hex(t);
		cout<<endl<<t;
	}



private:
	map<char,string> hex_to_binary_map;
	map<string,string> binary_to_hex_map;
	vector<string> key48bit_vector;
	int intial_permutation[64] = {58,50,42,34,26,18,10,2,
								 60,52,44,36,28,20,12,4,
								 62,54,46,38,30,22,14,6,
								 64,56,48,40,32,24,16,8,
								 57,49,41,33,25,17,9,1,
								 59,51,43,35,27,19,11,3,
								 61,53,45,37,29,21,13,5,
								 63,55,47,39,31,23,15,7};

	int inverse_intial_permutation[64] = {40,8,48,16,56,24,64,32,
										 39,7,47,15,55,23,63,31,
										 38,6,46,14,54,22,62,30,
										 37,5,45,13,53,21,61,29,
										 36,4,44,12,52,20,60,28,
										 35,3,43,11,51,19,59,27,
										 34,2,42,10,50,18,58,26,
										 33,1,41,9,49,17,57,25};

    int permuted_choice_one[56] = {57,49,41,33,25,17,9,   
  								  1,58,50,42,34,26,18,   
  								  10,2,59,51,43,35,27,   
  								  19,11,3,60,52,44,36,   
  								  63,55,47,39,31,23,15,   
  								  7,62,54,46,38,30,22,   
  								  14,6,61,53,45,37,29,   
  								  21,13,5,28,20,12,4};

  	int permuted_choice_two[48] = {14,17,11,24,1,5,3,28,  
 								  15,6,21,10,23,19,12,4,  
 								  26,8,16,7,27,20,13,2,  
 								  41,52,31,37,47,55,30,40,  
 								  51,45,33,48,44,49,39,56,  
 								  34,53,46,42,50,36,29,32};

 		int expansion_permutation[48]{32,1,2,3,4,5,
 																	4,5,6,7,8,9,
 																	8,9,10,11,12,13,
 																	12,13,14,15,16,17,
 																	16,17,18,19,20,21,
 																	20,21,22,23,24,25,
 																	24,25,26,27,28,29,
 																	28,29,30,31,32,1};

 		int permutation_p[32]={16,7,20,21,
 													 29,12,28,17,
 													 1,15,23,26,
 													 5,18,31,10,
 													 2,8,24,14,
 													 32,27,3,9,
 													 19,13,30,6,
 													 22,11,4,25};

 	int bit_rotates[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
 	int s_boxes[8][4][16] = {
 	 					{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                          0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                          4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                          15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },

                        { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                          3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                          0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                          13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
 
                        { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                          13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                          13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                          1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },

                        { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                          13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                          10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                          3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },

                        { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                          14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                          4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                          11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },

                        { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                          10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                          9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                          4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },

                        { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                          13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                          1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                          6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },

                        { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                          1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                          7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                          2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } };
 
};

int main(int argc, char **argv){
	string command = argv[1];
	string text = argv[2];
	string key = argv[3];
	DES_alg des_alg;

	if(command == "encrypt"){
		cout<<"cipher: "<<des_alg.encrypt(text,key)<<endl;

	}else if(command =="decrypt"){
		des_alg.encrypt(text,key);
		cout<<"plain: "<<des_alg.decrypt(text)<<endl;
	}
	
	//test.test_permute(str);
	//test.test_conv();
	//test.test_s_box();
}
