#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <math.h>

using namespace std;

//找到每個沒有don't care的Implicant
void findAllImplicants(string input, int index, vector<string>& implicants);

//將Implicant轉成minterm
set<int> implicantToMinterms(string implicant);

//依照1的數量做排序
void sortBy1Num(vector<string>& implicants);

//化簡Implicant成PI
void reduceImplicants(vector<string>& implicants);

//找到Essential Prime Implicant
vector<string> findEssentialPI(vector<string> implicents, vector<string> dontCares);

//Patrick method
vector<string> patrick(vector<string> notEssentialPI);

//組合 C n取m
void comb(int n, int m, int i, vector<string> list, vector<vector<string>>& result);

int main(int argc, char* argv[])
{

	// if (argc < 3)
	// {
	// 	cout << "arg error";
	// 	return 0;
	// }
	// char *inputFileName = argv[1];
	// char *outputFileName = argv[2];
	string inputFileName = "input.pla";
	string  outputFileName = "output.pla";

	fstream input(inputFileName);
	vector<string> implicants, essentialPI, dontCares;
	unordered_map<string, set<int>> implicantMintermPair, dontCaresMintermPair;
	vector<string> inputVar, outputVar;
	int inputNum = 0, outputNum = 0, productNum = 0;
	// read file and trasfer to obdd
	while (!input.eof())
	{
		string buf, op;
		getline(input, buf);
		stringstream ss(buf);
		ss >> op;

		if (op == ".i")
		{
			ss >> inputNum;
		}
		else if (op == ".o")
		{
			ss >> outputNum;
		}
		else if (op == ".ilb")
		{
			for (int i = 0; i < inputNum; i++)
			{
				ss >> buf;
				inputVar.push_back(buf);
			}
		}
		else if (op == ".ob")
		{
			for (int i = 0; i < outputNum; i++)
			{
				ss >> buf;
				outputVar.push_back(buf);
			}
		}
		else if (op == ".p")
		{
			ss >> productNum;
			for (int i = 0; i < productNum; i++)
			{
				char result;
				input >> buf;
				input >> result;
				findAllImplicants(buf, 0, implicants);
				if (result == '-')
				{
					findAllImplicants(buf, 0, dontCares);
				}
			}
		}
		else if (op == ".e")
		{
			break;
		}
		else
		{
			break;
		}
	}
	// Quine–McCluskey algorithm
	sortBy1Num(implicants);
	reduceImplicants(implicants);
	essentialPI = findEssentialPI(implicants, dontCares);

	// Petrick's method
		//去除essential PI
	for (int i=0;i<implicants.size();i++)
	{
		for (string epi : essentialPI)
		{
			if (epi == implicants[i])
			{
				implicants.erase(implicants.begin() + i);
				break;
			}
		}
	}
		



	for (string imp : implicants)
	{
		cout << imp << endl;
	}
	cout << endl
		<< "essential\n";
	for (string imp : essentialPI)
	{
		cout << imp << endl;
	}

	// fstream output(outputFileName, ios::out);
	// output.close();
	input.close();
}

void findAllImplicants(string input, int index, vector<string>& implicants)
{
	if (index == input.length())
	{
		implicants.push_back(input);
		return;
	}
	if (input[index] == '-')
	{
		input[index] = '0';
		findAllImplicants(input, index + 1, implicants);
		input[index] = '1';
		findAllImplicants(input, index + 1, implicants);
	}
	else
	{
		findAllImplicants(input, index + 1, implicants);
	}
}

set<int> implicantToMinterms(string implicant)
{
	vector<string> allImplicants;
	set<int> minterms;

	findAllImplicants(implicant, 0, allImplicants);

	for (int i = 0; i < allImplicants.size(); i++)
	{
		int minterm = 0;
		for (int j = 0; j < allImplicants[i].size(); j++)
		{
			if (allImplicants[i][allImplicants[i].size() - j - 1] == '1')
			{
				minterm += pow(2, j);
			}
		}
		minterms.insert(minterm);
	}

	return minterms;
}

void sortBy1Num(vector<string>& implicants)
{
	for (int j = 0; j < implicants.size() - 1; j++)
	{
		for (int i = 0; i < implicants.size() - 1; i++)
		{
			int countA = 0, countB = 0;
			for (int k = 0; k < implicants[i].length(); k++)
			{
				if (implicants[i][k] == '1')
					countA++;
				if (implicants[i + 1][k] == '1')
					countB++;
				if (countA > countB)
				{
					string temp = implicants[i];
					implicants[i] = implicants[i + 1];
					implicants[i + 1] = temp;
				}
			}
		}
	}
}

void reduceImplicants(vector<string>& implicants)
{
	bool allReduce = true;
	vector<string> result;
	for (int i = 0; i < implicants.size(); i++)
	{
		bool flag = false;
		for (int j = 0; j < implicants.size(); j++)
		{
			if (i == j)
				continue;
			int diff = 0;
			for (int k = 0; k < implicants[i].length(); k++)
			{
				if (implicants[i][k] != implicants[j][k])
				{
					// 計算兩個implicant差別幾個變數
					diff++;
					if (diff > 1)
						break;
				}
			}
			//如果可以合併
			if (diff == 1)
			{
				flag = true;
				for (int k = 0; k < implicants[i].length(); k++)
				{
					if (implicants[i][k] != implicants[j][k])
					{
						string temp = implicants[i];
						temp[k] = '-';
						int n = 0;
						//尋找是否有重複的implicant
						for (; n < result.size(); n++)
						{
							if (result[n] == temp)
								break;
						}
						if (n == result.size())
						{
							result.push_back(temp);
							allReduce = false;
							break;
						}
					}
				}
			}
		}
		//如果都沒有可以化簡
		if (!flag)
		{
			result.push_back(implicants[i]);
		}
	}

	if (!allReduce)
		reduceImplicants(result);

	implicants = result;
}

vector<string> findEssentialPI(vector<string> implicents, vector<string> dontCares)
{
	vector<string> essentialPI;
	unordered_map<string, set<int>> implicantMintermPair, dontCaresMintermPair;
	set<int> allMinterms;
	for (string imp : implicents)
	{
		implicantMintermPair[imp] = implicantToMinterms(imp);
	}
	for (string imp : dontCares)
	{
		dontCaresMintermPair[imp] = implicantToMinterms(imp);
	}
	for (const auto& val : implicantMintermPair)
	{
		for (int num : val.second)
		{
			allMinterms.insert(num);
		}
	}
	for (const auto& val : dontCaresMintermPair)
	{
		for (int num : val.second)
		{
			allMinterms.erase(num);
		}
	}

	for (int min : allMinterms)
	{
		int count = 0;
		for (const auto& val : implicantMintermPair)
		{
			for (int num : val.second)
			{
				if (num == min)
					count++;
			}
		}
		if (count == 1)
		{
			for (const auto& val : implicantMintermPair)
			{
				for (int num : val.second)
				{
					if (num == min)
					{
						essentialPI.push_back(val.first);
					}
				}
			}
		}
	}
	return essentialPI;
}

vector<string> patrick(vector<string> notEssentialPI)
{

}

void comb(int n, int m, int i, vector<string> list,vector<vector<string>> &result)
{
	for (int j = i; j < list.size() - m + 1;j++)
	{
	
	}
}