//Modules.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

//Main data structure to contain the data for the skills.
struct NodeData{
	std::string name;
	std::string difficulty;
	short progress;
	short stage = 0;
	bool flag = false;
	bool done = false;
	std::string parentname;
};

//Initializing some default/global values used in the program.
NodeData defaultval;
bool fakeStop = false;
std::string globalDataName = "data.txt";

//An algorithm that checks if one word is in another, used for determining if the user wants to stop and/or continue.
bool inString(std::string word, std::string word2){
	std::string tempword = "";
	for (char character:word){
		if (!(character > 47 && character < 58)){
			tempword += std::tolower(character);
		}
	}
	if (tempword == word2){
		return true;
	}
	return false;
}

// Used to check if a string is numeric or not.
bool isNumeric(std::string str){
	short count;
	if (str[0] == '-'){count = 1;}
	else {count = 0;}
	for (short i = count;i < str.size();++i){
		if (!std::isdigit(str[i])){
			return false;
		}
	}
	return true;
}

//Filters whitespace and returns the new value.
std::string filterWSpace(std::string word){
	std::string newword = "";
	for (auto character:word){
		if ((int)character > 32){
			newword += character;
		}
	}
	return newword;
}

//Prompts the user to enter a value which is non empty.
void emptyValidator(std::string* strdata, std::string prompt){
	bool not_empty = false;
	std::string str;
	while (!not_empty){
		std::cout << prompt;
		std::getline(std::cin, str);
		if (filterWSpace(str).size() != 0){
			not_empty = true;
		}
	}
	*strdata = str;
}

//Given information about a node, prints the initialization message.
void showData(NodeData data){
	std::cout << "Base skill called: " << data.name << "\n";
	std::cout << "Base skill has a difficulty of: " << data.difficulty << "\n";
	std::cout << data.name << " has been initialized" << "\n" << std::endl;
}

//Prompts the user to a feature value for a skill.
template <typename T> bool enterFeature(T* data,std::string prompt,short count){
	bool done = false;
	std::string tempdata;
	bool stopping;
	while (!done){
		emptyValidator(&tempdata, prompt);
		stopping = inString(tempdata, "stop");
		if (stopping and count > 0){return true;}
		else if (count == 0 && stopping){std::cout << "You need at least one base skill" << std::endl;}
		else {*data = tempdata;done=true;}
	}
	return false;
}

//Prompts the user to enter skill information.
NodeData requestNode(bool* stopping = &fakeStop, short count = 1){
	NodeData data;
	if(enterFeature<std::string>(&data.name, "Enter the name for your skill: ", count)){*stopping=true;return defaultval;};
	if(enterFeature<std::string>(&data.difficulty, "Enter the difficulty for your skill: ", count)){*stopping=true;return defaultval;};
	return data;
}

//Class definition to store information and operations associated with the skills.
class Node {
public:
	std::vector<Node> Children;
	std::string Name;
	std::string difficulty;
	NodeData data;
	
	//Constructor that assigns information inside the function to parameters.
	Node(NodeData currentData):data(currentData){
		Name = currentData.name;
		difficulty = currentData.difficulty;
	}

	//Checks if a skill with a name arg is in the skill tree.
	Node* inChildren(std::string name){
		Node* it = &this->Children[0];
		for (unsigned short i = 0; i < this->Children.size(); ++i){
			if (it->Name==name){
				it->data.flag = true;
				return it;
			}
			it++;
		}
		return this;
	}

	//Traverses the tree.
	Node* processRoot(Node* root, std::string name){
		Node* condition = root->inChildren(name);
		if (*condition){
			return condition;
		}
		for (auto& element:root->Children){
			Node* foundnode = processRoot(&element, name);
			if (*foundnode){
				return foundnode;
			}
		}
		return this;
	}

	//Finds a skill in the skilltree given a vast array of arguments.
	Node* findNode(std::string prompt="", bool mode = false, bool mainMode = false, std::string nameArg = ""){
		std::string name;
		bool found = false;
		while (!found){
			if (!mainMode){
				std::cout << prompt;
		        std::getline(std::cin, name);
			}
			else {
				name = nameArg;
			}
			if (mode && name.size() == 0){
				return this;
			}
			if (inString(name, "stop")){break;}
			Node* isFoundNode = this->processRoot(this, name);
			if (*isFoundNode){
				return isFoundNode;
			}
			else {
				std::cout << "Skill has not been found in your skill tree. Please try again.\n";
			}
		}
		return this;
	}
	
	//Adds a skill to the skill tree.
	void Push(std::vector<Node>* rootNodes){
		NodeData newData;
		newData = requestNode();
		Node item(newData);
		Node* isFoundNode = this->findNode("Under what skill would you like this new skill to be under: ");
		if (*isFoundNode){
			std::cout << "\nSkill found! Your new skill has been saved.\n";
			isFoundNode->Children.push_back(item);
			isFoundNode->data.flag = false;
		}
	}
	
	//Deletes a skill from the skill tree.
	void Delete(std::vector<Node>* rootNodes){
		bool valid = false;
		while (!valid){
			Node* parentDelete = this->findNode("What skill is your skill to delete directly under? If this skill is a root, leave blank: ", true);
			Node* nodeToDelete = this->findNode("What skill would you like to delete: ");
			std::vector<Node>::iterator it = std::find(parentDelete->Children.begin(), parentDelete->Children.end(), *nodeToDelete);
			if (it != parentDelete->Children.end()){
				std::cout << "DELETED: " << nodeToDelete->data.name << (char)10;
				parentDelete->Children.erase(it);
				nodeToDelete->data.flag = false;
				parentDelete->data.flag = false;
				valid = true;
			}
			else {
				std::cout << "No skill called: " << nodeToDelete->data.name << " is under the skill" << parentDelete->data.name << (char)10;
			}
		}
	}

	//Marks a node in the tree as done/finished.
	void Done(std::vector<Node>* rootNodes){
		Node* isFoundNode = this->findNode("Which node would you like to mark as done: ");
		if (*isFoundNode){
			std::cout << "\nSkill found! Your skill has now been marked as finished.\n";
			isFoundNode->data.done = true;
			isFoundNode->data.flag = false;
		}
	}

	//Intermediary function to exit back one layer of depth in the program.
	void Exit(std::vector<Node>* rootNodes){
		std::cout << "Consider saving your data." << (char)10;
	}

	//Overloaded operator to evaluate skills as true based on a flag variable in it's data.
	explicit operator bool() const {
		return this->data.flag;
	}

	//Overloading the equal operator to find skills in a list of nodes, used for deletion primarily.
	bool operator==(const Node secondNode){
		return this->Name==secondNode.Name;
	}
	

};

//Base skills of which all other skills branch off of.
NodeData basedata;
Node baseRoot(basedata);

//A means to compare two nodes by their stage, which is some non-negative integer value determining their depth in their tree.
bool Compare(const Node &lhs, const Node &rhs){
	return lhs.data.stage < rhs.data.stage;
}

//Substring of a string, given two endpoints.
std::string subStringIndicies(std::string param, int pos1, int pos2){
	int length = (pos2 > pos1) ? (pos2-pos1):0;
	return param.substr(pos1, length+1);
}

//Extracts data from a string in a certain format.
std::vector<std::string> Extract(std::string arg){
	std::vector<std::string> elements = {};
	std::vector<int> indexes = {};
	for (unsigned short i = 0; i < arg.size(); ++i){
		if (arg[i]==' '){
			if (arg[i+1] == '{' || arg[i-1] == '}'){
				indexes.push_back(i);
			}
		}
	}
	if (indexes.size() == 0){return elements;}
	std::string currentelement = "";
	for (unsigned short i = 0; i < indexes.size(); ++i){
		if (i != indexes.size()-1){elements.push_back(subStringIndicies(arg,indexes[i]+2, indexes[i+1]-2));}
		else {elements.push_back(subStringIndicies(arg,indexes[i]+2, arg.size()-2));}
	}
	elements.push_back(subStringIndicies(arg,1,indexes[0]-2));

	return elements;
}

//Returns a subvector given a vector and two endpoints.
template <typename T> std::vector<T> retrieveFromBounds(std::vector<T> mainvec,int bound1, int bound2){
	std::vector<T> toReturn = {};
	for (unsigned short i = bound1; i <= bound2; ++i){
		toReturn.push_back(mainvec[i]);
	}
	return toReturn;
}

//Clears the data file.
void Clear(bool* selectedroots, Node* baseroot, std::ofstream* writestream, bool mode = false){
	if (!mode){
		std::cout << "CLEARING";
	    *selectedroots = false;
		std::vector<Node> emptyVec = {};
	    for (auto node: baseroot->Children){
			node.Children = emptyVec;
		}
	}
	std::filesystem::remove(globalDataName);
}

//Given data, forms a string in some format which can be extracted later.
std::string fillTemplate(std::string name, std::string diff, std::string stage, std::string parentname, bool done){
	std::string entry = "";
	entry += "{"+name + "} ";
	entry += "{"+diff + "} ";
	entry += "{"+stage + "} ";
	entry += "{"+parentname + "} ";
	if (done){entry += "{1}\n";}
	else {entry += "{0}\n";}
	return entry;
}

//Saves the data collected in a session.
void Save(bool* selectedroots,Node* originalroot, std::ofstream* savestream, std::vector<Node>* nodelist, std::ifstream* readstream){
	std::string currentNodeInfo;
	std::vector<std::string> previous = {};
	while (std::getline(*readstream, currentNodeInfo)){
		if (currentNodeInfo == ""){break;}
		if ((int)currentNodeInfo[0] >= 48 && (int)currentNodeInfo[0] <= 57){continue;}
		std::vector<std::string> extracted = Extract(currentNodeInfo);
		currentNodeInfo = fillTemplate(extracted[4], extracted[0], extracted[1], extracted[2], std::stoi(extracted[3]));
		previous.push_back(currentNodeInfo);
	}
	std::string savedata = "";
	Clear(selectedroots, originalroot, savestream, true);
	std::ofstream temporaryStream(globalDataName);
	if (*selectedroots){
		temporaryStream << "1" << std::endl;
	}
	else {
		temporaryStream << "0" << std::endl;
	}
	for (auto node:*nodelist){
		savedata = fillTemplate(node.data.name, node.data.difficulty, std::to_string(node.data.stage), node.data.parentname, node.data.done);
		if (std::find(previous.begin(), previous.end(), savedata) != previous.end()){
			continue;
		}
		temporaryStream << savedata;
		savedata = "";
	}
}

//Loads the data from the text file before starting a new session.
void Load(Node* baseroot, bool* rootsSelected){
	std::ifstream SkillTree(globalDataName);
	std::string currentNodeInfo;
	std::vector<Node> nodes = {};
	while (std::getline(SkillTree, currentNodeInfo)){
		if (currentNodeInfo == ""){break;}
		if ((int)currentNodeInfo[0] >= 48 && (int)currentNodeInfo[0] <= 57){
			if ((int)currentNodeInfo[0] != 48){*rootsSelected = true;}
			continue;
		}
		NodeData currentNodeData;
		std::vector<std::string> extractedData = Extract(currentNodeInfo);
		currentNodeData.difficulty = extractedData[0];
		currentNodeData.stage = std::stoi(extractedData[1]);
		currentNodeData.name = extractedData[4];
		currentNodeData.parentname = extractedData[2];
		currentNodeData.done = std::stoi(extractedData[3]);
		Node newNode(currentNodeData);
		nodes.push_back(newNode);
	}
	if (nodes.size() > 0){
		std::sort(nodes.begin(), nodes.end(), Compare);
		for (unsigned short i = 0; i < nodes.size(); ++i){
			if (nodes[i].data.stage == 1){
				baseroot->Children.push_back(nodes[i]);
			}
			else {
				Node* FoundNode = baseroot->findNode("",false,true,nodes[i].data.parentname);
				if (FoundNode){
					FoundNode->Children.push_back(nodes[i]);
					FoundNode->data.flag = false;
				}
			}
		}
	}
}

//Asks the user to enter a choice given a mapping detailing choices and their respective values that must be entered for them to select that choice.
int invokeUser(std::vector<std::string> QuestionMapping, bool mode = false){
	std::string choice = "";
	if (!mode){
		std::string prompt = "\n";
		for (unsigned short i = 0; i < QuestionMapping.size();i+=2){
			prompt += QuestionMapping[i] + ". " + QuestionMapping[i+1] + "\n";
		}
		std::cout << prompt << std::endl;
		std::getline(std::cin, choice);
		std::vector<std::string>::iterator it = std::find(QuestionMapping.begin(), QuestionMapping.end(), choice);
		if (it != QuestionMapping.end()){
			return std::stoi(*it);
		}
		else {
			return -1;
		}
	}
	else {
		std::cout << "Please re-enter your choice: ";
		std::getline(std::cin, choice);
		std::vector<std::string>::iterator it = std::find(QuestionMapping.begin(), QuestionMapping.end(), choice);
		if (it != QuestionMapping.end()){
			return std::stoi(*it);
		}
		else {
			return -1;
		}
	}
}

//Builds the base/fundemental skills of the skill tree. Also contains the second layer of depth.
void StartBuilding(bool* selectedroots, Node* baseroot, std::ofstream* writestream, bool mode = false){
	int choice;
	std::vector<Node> roots = {};
	if (!*selectedroots){
		short count = 0;
		NodeData currentData;
		std::string tempdata;
		std::cout << "Now you will select your base skills, roots that your other skills will branch off of, enter stop at anytime to stop" << std::endl;
		bool stopping = false;
		while (!stopping){
			currentData = requestNode(&stopping, count);
			if (stopping){break;}	
			std::cout<<"\n";showData(currentData);
			std::cout << "Would you like to enter another? ";
			std::getline(std::cin, tempdata);
			stopping = inString(tempdata, "stop");
			Node root(currentData);
			roots.push_back(root);
			baseroot->Children.push_back(root);
			count++;
			*selectedroots = true;
			if (inString(tempdata, "no")){break;}
		}
	}
	else {
		bool rootchoice = false;
		bool decider = false;
		std::string rootans = "";
		std::cout << "Would you like to enter any base nodes: ";
		std::getline(std::cin, rootans);
		if (inString(rootans, "no")){decider = true;}
		short count = 1;
		NodeData currentData;
		std::string tempdata;
		bool stopping = false;
		while (!stopping && !decider){
			currentData = requestNode(&stopping, count);
			if (stopping){break;}	
			std::cout<<"\n";showData(currentData);
			std::cout << "Would you like to enter another? ";
			std::getline(std::cin, tempdata);
			stopping = inString(tempdata, "stop");
			Node root(currentData);
			roots.push_back(root);
			baseroot->Children.push_back(root);
			if (inString(tempdata, "no")){break;}
			count++;
		}	
	}
	std::vector<std::string> buildMapping = {"1", "Add Node", "2", "Delete Node", "3", "Mark a node as done", "4", "Exit"};
	int exitnum = std::stoi(buildMapping[buildMapping.size()-2]);
	std::vector<std::function<void(std::vector<Node>*)>> buildFunctions = {[=](std::vector<Node>* bT){baseroot->Push(bT);}, [=](std::vector<Node>* bT){baseroot->Delete(bT);}, [=](std::vector<Node>* bT){baseroot->Done(bT);},[=](std::vector<Node>* bT){baseroot->Exit(bT);}};
	while (choice != exitnum){
		choice = invokeUser(buildMapping);
		while (choice < 0){
			choice = invokeUser(buildMapping, true);
		}
		buildFunctions[choice-1](&roots);
	}
}

//Traverses through the tree and in doing so builds up a vector object with information about the skills in the skill tree.
void buildTree(Node* baseRoot, std::vector<Node>* nodelist, short stage = 0, Node* parentnode = nullptr){
	if (stage>0){baseRoot->data.stage=stage;baseRoot->data.parentname = parentnode->data.name;nodelist->push_back(*baseRoot);}
	for (auto element:baseRoot->Children){
		buildTree(&element, nodelist, stage+1, baseRoot);
	}
}

//Presents information about a skill.
void presentNode(Node item){
	std::cout << (char)10;
	std::cout << "NAME: " << item.data.name << std::endl;
	std::cout << "DIFFICULTY: " << item.data.difficulty << std::endl;
	std::cout << "PARENT: " << item.data.parentname << std::endl;
	if (item.data.done){
		std::cout << "STATUS: " << "DONE" << std::endl;
	}
	else {
		std::cout << "STATUS: " << "NOT DONE" << std::endl;
	}
	std::cout << (char)10;
}

//Allows the user to see the full scope of the skill tree.
void ViewTree(bool* selectedroots, Node* baseroot, std::ofstream* writestream, bool mode = false){
	std::string treeString;
	std::vector<Node> items = {};
	buildTree(baseroot, &items);
	for (auto element: items){
		presentNode(element);
	}
}

//Exits the program, also allows the user to save their data before exiting.
void Exit(bool* selectedroots, Node* baseroot, std::ofstream* WriteStream, bool mode = false){
	std::ifstream readstream(globalDataName);
	std::vector<Node> nodes = {};
	std::string savechoice;
	std::cout << (char)10 << "Would you like to save your data: ";
	std::getline(std::cin, savechoice);
	if (!inString(savechoice, "no")){
		std::cout << "Saving...\n";
		buildTree(baseroot, &nodes);
		Save(selectedroots,baseroot, WriteStream, &nodes, &readstream);
	}
}

//Main function, first layer of depth in the program.
int main(){
	baseRoot.data.name = "This node is a root node.";
	int choice;
	std::string data;
	bool selectedroots = false;
	Load(&baseRoot, &selectedroots);
	std::vector<std::string> InitialQuestionMapping 
		= {"1", "Build/Update Tree", "2", "View Tree", "3", "Clear Data", "4", "Exit"};
	std::vector<std::function<void(bool*, Node*, std::ofstream*, bool)>> baseFunctions = {StartBuilding, ViewTree, Clear, Exit};
	short exitnum = std::stoi(InitialQuestionMapping[InitialQuestionMapping.size()-2]);
	while (choice != exitnum){
		std::ofstream WriteData(globalDataName, std::ios::app);
		choice = invokeUser(InitialQuestionMapping);
		while (choice < 0){
			choice = invokeUser(InitialQuestionMapping, true);
		}
		baseFunctions[choice-1](&selectedroots, &baseRoot, &WriteData, false);
	}
	return 0;
}
