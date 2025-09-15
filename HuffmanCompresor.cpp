// HuffmanCompresor.cpp : This file contains the 'main' function. Program execution begins and ends there.
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


#include <iostream>
#include <string>
#include<fstream>
#include <unordered_map>
#include <queue>
#include <functional>
#include <sstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <bitset>
#include <vector>
#include <codecvt>
#include <locale>
#include <io.h>
#include<fcntl.h>
#include<Windows.h>
#include <thread>
#include <future>
using namespace std;


static std::unordered_map<char, int> read(std::ifstream& inputfile) {
	std::unordered_map<char, int> saved_elem;

	char znak;
	while (inputfile.get(znak)) {
		saved_elem[znak]++;
	}

	return saved_elem;
}

struct Node {

	char ch;
	int freq;
	Node* left;
	Node* right;

	Node(char c, int f, Node* l = nullptr, Node* r = nullptr) : ch(c), freq(f), left(l), right(r) {

	}

	

};

struct Compare {

	bool operator()(Node* a, Node* b) {

		return a->freq > b->freq;
	}

};


Node* buildHuffmanTree(std::unordered_map<char, int> map_of_chars) {

	std::priority_queue<Node*, std::vector<Node*>, Compare> queue;

	//vytvorenie listov
	for (auto& pair : map_of_chars) {
		Node* newNode = new Node(pair.first, pair.second);
		queue.push(newNode);
	}

	//spajanie kym nezostane jeden uzol 
	while (queue.size()>1) {

		Node* left = queue.top(); 
		queue.pop();

		Node* right = queue.top();
		queue.pop();

		auto freq_ = left->freq + right->freq;
		Node* parent = new Node('\0', freq_, left, right);

		queue.push(parent);
		
	}
	

	return queue.top();
}


void freeTree(Node* root) {
	if (!root) {
		return;
	}

	freeTree(root->left);
	freeTree(root->right);

	delete root;
}

void generateCodes(Node* root, const std::string& prefix, std::unordered_map<char, std::string>& codes) {

	if (!root) {
		return;
	}

	if (root->ch != '\0') {
		codes[root->ch] = prefix;
	}

	generateCodes(root->left, prefix + "0", codes);
	generateCodes(root->right, prefix + "1", codes);
}

std::string encode(const std::string& text , std::unordered_map<char, std::string>& codes) {
	std::string encoded;
	for (char c : text) {
		encoded += codes.at(c);
	}
	return encoded;
}

std::string decode(const std::string& bit_text, Node* root) {
	std::string decoded;
	Node* currentRoot = root;

	for (auto elem : bit_text) {

		if (elem == '0')
		{
			currentRoot = currentRoot->left;
		}
		else {
			currentRoot = currentRoot->right;
		}
		if (currentRoot->ch != '\0') {
			decoded += currentRoot->ch;
			currentRoot = root;
		}
	}
	return decoded;
}


int spustacia_fcia1() {
	std::ifstream input("input_skuska.txt");

	if (!(input.is_open())) {
		cerr << "Nepodarilo sa otvorit subor(zly nazov alebo neexistuje)" << endl;
		return 1;
	}

	auto freq = read(input);

	Node* root = buildHuffmanTree(freq);



	std::unordered_map<char, std::string> codes;
	generateCodes(root, "", codes);

	std::ifstream input2("input_skuska.txt");
	std::stringstream buffer;

	buffer << input2.rdbuf();
	std::string text = buffer.str();

	std::string encoded = encode(text, codes);

	std::string decoded = decode(encoded, root);


	cout << "===================ORIGINAL===================\n" << endl;
	cout << text << endl << endl;
	cout << "===================ENCODED===================\n" << endl;
	cout << encoded << endl << endl;
	cout << "===================DECODED===================\n" << endl;
	cout << decoded << endl << endl;

	std::cout << "===== Huffman Codes =====\n";
	for (auto& pair : codes) {
		std::cout << "'" << pair.first << "' : " << pair.second << "\n";
	}
	std::cout << "=========================\n";


	freeTree(root);
	input.close();
	input2.close();
	return 0;
}


















///////////////////////////////////////////////////////////////////////////////////////
/*

			POKUS 2 - HUFFMAN COMPRESSOR WITH UNIQUE POINTERS
			 


*/



std::u32string utf8_to_u32(const std::string& utf8) {

	//prvy argument sluzia na prevadzanie utf-8 a utf-32(char32_t)
	//druhy argument urcuje vnutorny znakovy typ s ktorym bude wstring pracovat v tomto pripade je UTF-32 == char32_t
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	
	//from_bytes lebo ideme z utf8 do utf32
	return conv.from_bytes(utf8);

}

std::string u32_to_utf8(const std::u32string& u32) {

	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	
	//from_bytes lebo ideme z utf32 do utf8
	return conv.to_bytes(u32);

}




void makeReportFile(const std::string& message) {

	
	std::ofstream reportFile("REPORT_MESSAGE.txt", std::ios::out | std::ios::app);

	if (!(reportFile.is_open())) {
		std::cerr << "NEPODARILO SA VYTVORIT REPORT_MESSAGE.txt\n";
		std::exit(EXIT_FAILURE);
	}

	reportFile << message << endl;
	reportFile.close();

	cerr << "\n================================= POZOR NASTALA CHYBA POZRI REPORT_MESSAGE.txt FILE =================================\n" << endl;

	std::exit(EXIT_FAILURE);

}




//musim najprv mat klasicky string a potom ho konvertovat lebo funkcia ktora cita subory tak cita len bajty
//ale mozem spravit aj u32string len to bude returnovat vysledok z convertovacej fcie
static std::u32string readUtf8fileAsu32(const std::string& filename) {

	std::ifstream inputfile(filename, std::ios::binary);

	if (!(inputfile.is_open())) {
		makeReportFile("Nepodarilo sa otvorit subor ::readUtf8FileAsu32");
	}

	std::stringstream buff;
	buff << inputfile.rdbuf();

	return utf8_to_u32(buff.str());
}






template<typename container> 
auto getFreqGeneric(const container& data) {


	using Symbol = typename std::decay<decltype(*std::begin(data))>::type;


	std::unordered_map<Symbol, int> map;

	for (auto const &ch : data) {
		map[ch]++;
	}

	return map;
}


template<typename Symbol>
struct links {

	int freq;
	Symbol c;
	std::unique_ptr<links<Symbol>> left ;
	std::unique_ptr<links<Symbol>> right ;

	links(Symbol c_, int f_, std::unique_ptr<links> left_ = nullptr, 
		std::unique_ptr<links> right_ = nullptr): c(c_) , freq(f_),left(std::move(left_)), right(std::move(right_))  {

	}

	bool isLeaf() const {
		return !left && !right;
	}


};

template<typename Symbol>
struct my_Compare {

	bool operator()(const std::unique_ptr<links<Symbol>>& a, const std::unique_ptr<links<Symbol>>& b) const{
		return a->freq > b->freq;
	}

};

template<typename Symbol>
std::unique_ptr<links<Symbol>> popTop(std::priority_queue< std::unique_ptr<links<Symbol>>, std::vector< std::unique_ptr<links<Symbol>>> , my_Compare<Symbol>>& g ){

	auto ptr = std::move(const_cast<std::unique_ptr<links<Symbol>>&>(g.top())); //std::move(g.top()); bezpecnejsia verzia ale nejde mi
	g.pop();
	return ptr;

}

template<typename Symbol>
std::unique_ptr<links <Symbol>> buildTree(const std::unordered_map<Symbol,int>& map) {

	std::priority_queue<std::unique_ptr<links<Symbol>>, std::vector<std::unique_ptr<links<Symbol>>>, my_Compare<Symbol>> queue;

	for (auto& elem : map) {
		auto newLink = std::make_unique<links<Symbol>>(elem.first, elem.second);
		queue.push(std::move(newLink));
	}

	if (queue.empty()) {
		return nullptr;
	}

	while (queue.size() > 1) {

		auto left = popTop(queue);
		

		auto right = popTop(queue);
		
		auto freq = left->freq + right->freq;
		auto parent = std::make_unique<links<Symbol>>(Symbol(), freq, std::move(left), std::move(right));

		queue.push(std::move(parent));
	}
	return popTop(queue);
}

// buildTree ale s codmi

template<typename Symbol>
std::unique_ptr<links<Symbol>> buildTreeFromCodes(const std::unordered_map<Symbol,std::string>& codes) {

	auto root = std::make_unique<links<Symbol>>(Symbol(), 0); ////////// pozor !! Symbol() je defaultny construktor ktori ti vrati hodnotu podla typu premenej cize ak je to int = 0 , unsigned char = '\0', char32_t = U'\0'  
	for (const auto& p : codes) {
		links<Symbol>* curr = root.get();
		for (char b : p.second) {
			if (b == '0')
			{
				if (!(curr->left)) {
					curr->left = std::make_unique<links<Symbol>>(Symbol(), 0);
				}
				curr = curr->left.get();

			}
			else {
				if (!(curr->right)) {
					curr->right = std::make_unique<links<Symbol>>(Symbol(), 0);
				}
				curr = curr->right.get();
			}
		}
		curr->c = p.first; //ulozenie znaku v listovom uzle 
	}
	return root;
}

template <typename Symbol>
void  GenerateCodes2(const std::unique_ptr<links<Symbol>>& root ,const std::string& prefix, std::unordered_map<Symbol , std::string>& codes) {

	if (!root) {
		return;
	}

	if (!root->left && !root->right) {
		codes[root->c] = prefix.empty() ? "0" : prefix;
	}
	else {
		GenerateCodes2(root->left, prefix + "0", codes);
		GenerateCodes2(root->right, prefix + "1", codes);
	}
}


template< typename Container,typename Symbol = typename Container::value_type >
std::string encode2(const Container& data, const unordered_map<Symbol, std::string>& codes) {

	std::string encoded;


	encoded.reserve(data.size() * 3); //rezervovanie pamati optimalizacia
	for (const Symbol &c : data) {

		encoded += codes.at(c); //vracia binarny retazec daneho znaku
	}

	return encoded; //vracia string 
}


std::vector<unsigned char> encodedBlock(const std::vector<unsigned char>& data, size_t start , size_t end, const unordered_map<unsigned char, std::string>& codes) {



	std::vector<unsigned char> out;
	unsigned char buffer = 0;
	int bitCount = 0;

	for (size_t i = start; i < end; ++i) {

		const std::string& code = codes.at(data[i]);
		for (char b : code) {
			buffer = (buffer << 1 | b == '1');
			bitCount++;

			if (bitCount == 8) {//ak mame plny byte

				out.push_back(buffer);
				buffer = 0;
				bitCount = 0;
			}
		}
	}

	//ak zostali nepouzite bity
	if (bitCount > 0) {
		buffer = buffer << (8 - bitCount);
		out.push_back(buffer);
	}

	return out;



}




std::vector<unsigned char> encode2fastThread(const std::vector<unsigned char>& data, const unordered_map<unsigned char, std::string>& codes) {


	size_t nThreads = std::thread::hardware_concurrency(); //automaticky pouzije pocet jadier Cpu
	if (nThreads == 0) {
		cout << "\t\t\tNepodarilo sa zistit pocet jadier" << endl;
		nThreads = 4;
	}
	
	size_t blockSize = data.size() / nThreads;
	std::vector<std::future<std::vector<unsigned char>>> futures; //vector ktory ziskava buduce informacie typu vector uns char 
	
	//praca jednotlivych vlakien
	for (size_t t = 0; t < nThreads; ++t) {

		size_t start = blockSize;
		size_t end = (t== nThreads-1) ? data.size() : (t+1)*blockSize;

		//							asynchronne vlakno , spustana fcia, referenc. na data, idexy bloku pre vlakno, refern na codes
		futures.push_back(std::async(std::launch::async, encodedBlock, std::ref(data), start, end, std::ref(codes)));
	}

	//spojenie vysledkov

	std::vector<unsigned char> finalOutput;

	for (auto& f : futures) {
		std::vector<unsigned char> part;
		finalOutput.insert(finalOutput.end(), part.begin(), part.end());
	}


	return finalOutput;
}



std::vector<unsigned char> packBits(const std::string& bitString/*0011010...*/) {

	std::vector<unsigned char> bytes;


	for (size_t i = 0; i < bitString.size(); i += 8) {
		std::string byteStr = bitString.substr(i, 8);
		
		if (byteStr.size() < 8) {
			byteStr.append(8 - byteStr.size(), '0');
		}
		std::bitset<8> b(byteStr);
		//je najjednoduchší a najbezpečnejší spôsob, ako explicitne povedať kompilátoru „viem, že z 32-bit unsigned long robím 8-bit unsigned char“.
		bytes.push_back(static_cast<unsigned char>(b.to_ulong())); //da sa aj bytes.push_back(b.to_ulong()); ale viac chyb to bude mat

	}
	return bytes;
}

void writeBinaryFiles(const std::string fileName, const std::vector<unsigned char>& data) {

	std::ofstream outPut(fileName, std::ios::binary);

	for (auto ele : data) {
		outPut.put(ele);
	}
}

std::string unpackBits(const std::vector<unsigned char>& bytes,size_t validBits) {

	std::string bitString;
	size_t bitCount = 0;
	for (auto b : bytes) {
		for (int i = 7; i >= 0; --i) {
			if (bitCount >= validBits) {
				/*robi to ze ak mame 8 (0-7)bitov encoded texte tak vlastne tato podmienka breakne cyklus a nedoplni sa to
				0 do tych 8 bitov*/
				break;
			}
				bitString += ((b >> i) & 1) ? '1' : '0';
				bitCount++;
			
		}
	}
	return bitString;

}



template<typename Container ,typename Symbol = typename Container::value_type>
Container decode2(const std::string& encoded_text, const std::unique_ptr<links<Symbol>>& root){

	Container decoded;
	links<Symbol>* current = root.get();

	
	for (auto elem : encoded_text) {

		if (!current) {
			makeReportFile("decode2:: traversovanie null pointeru!" );
		}


		if (elem == '0') {
			current = current->left.get();
		}
		else {
			current = current->right.get();
		}

		if (current->isLeaf()) {
			decoded.push_back(current->c);
			current = root.get();
		}
	}
	return decoded;
}

/*

			# dekodovanie s vyuzitim multi-thread je zlozitejsie lebo prechadzam bit po bite stromom a ked som uprostred bajtu neviem ci uz mam cely znak alebo este ne 

			# preto treba rozdelit zakodovany bitovy stream na velke bloky a nasledne k jednotlivym blokom treba pridat nejaky OVERLAP napr. 32bajtov uplne dostacujuce


*/

template<typename Container, typename Symbol = typename Container::value_type>
Container decode2fast(const std::vector<unsigned char> &encodedData, size_t start , size_t end, links<Symbol>* root) {

	Container decoded; //dekodovane data
		
	links<Symbol>* current = root; //koren stromu 


	for (size_t i = start; i < end; ++i) { //od zaciatku az po koniec pre dany thread
		unsigned char byte= encodedData[i]; //actual bajt
		
		//prechadzame od najvyzsieho (7) po najnizsi(0)
		for (int bit = 7; bit >= 0; --bit) {
			int b = (byte >> bit) & 1; //vyextrahujeme konkretny bit ... nakresli si
			
			//posuvanie sa v strome podla hodnoty
			current = b ? current->right.get() : current->left.get();
			
			//ak sme na liste tak sme nasli cely symbol
			if (current->isLeaf()) {
				decoded.push_back(current->c);
				current = root;
			}

		}
	}
	return decoded;
}




template<typename Container, typename Symbol = typename Container::value_type>
Container decode2FastThread(const std::vector<unsigned char>&  encodedData, const std::unique_ptr<links<Symbol>>& root , size_t originalSize) {

	int numberOfThreads = std::thread::hardware_concurrency();

	if (numberOfThreads == 0) {
		cout << "\t\tnepodarilo sa zistit pocet jadier ::decode2FastThread";
		numberOfThreads = 4; //defaultne 4 vlakna 
	}

	size_t chunkSize = encodedData.size() / numberOfThreads; //rozdelenie dat podla poctu jadier
	size_t overlap = 32; //overlap ktory budeme pridavat 

	std::vector<std::future<Container>> futures; //buduce vysledky z threadov

	links<Symbol>* rawPtr = root.get(); //ukazovatel na koren huffman stromu


	//praca pre kazde vlakno
	for (int t = 0; t < numberOfThreads; ++t) {

		size_t start = t * chunkSize;
		size_t end = (t == numberOfThreads - 1) ? encodedData.size() : (t + 1) * chunkSize;

		if (start > 0) {
			start -= overlap; //zvacsujem o 32 bajtov az ked za nultym 
		}
		if (end < encodedData.size()) {
			end += overlap; 
		}
			//				& - zachyti vsetky ostatne premenne odkazom ,  start, end → tieto konkrétne hodnoty vezmeme ako kópiu (aby sa nezmenili v cykle)
		futures.push_back(std::async(std::launch::async, [&, start, end]() { return decode2fast<Container, Symbol>(encodedData, start, end, rawPtr); }));

	}


	Container result;
	result.reserve(originalSize); //rezervacia pamata dopredu zvysenie rychlosti

	//prechadzmae vysledkami z vlakien a spajame ich 
	for (auto& f : futures) {
		Container part = f.get();
		result.insert(result.end(), part.begin(), part.end());
	}

	//ak vysledok obsahuje viac symbolov nez original - orezeme 
	//kvoli overlapu
	if (result.size() > originalSize) {
		result.resize(originalSize);
	}


	return result;
}







template <typename Symbol , typename Container>
void printStatistic(std::unordered_map<Symbol,int> freq , std::unordered_map<Symbol,std::string> codes , const Container& origData ,bool isText = true) {

	//vypocet poctu znakov 
	int totalChars = 0;

	for (const auto& e : freq) {
		totalChars += e.second;
	}
	cout << " \n			========== TOTAL CHARS : " << totalChars <<" ===========\n" << endl;

	if (isText) {
		//vypocet percentualneho vyskutu znaku
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;


		for (const auto& p : freq) {
			std::string utf8Char = conv.to_bytes(p.first); //konverzia lebo cout ma problem s char32t

			double percentage = static_cast<double> (p.second) / totalChars * 100;
			cout << "' " << utf8Char << " ' ,count of " << p.second << " ,have: ( " << percentage << " %)\n";
		}
	}
	/*
	else {
		
		
		for (const auto& p : freq) {
			double percentage = static_cast<double> (p.second) / totalChars * 100;
			cout << "Byte[ " << static_cast<int>(static_cast<unsigned char> (p.first)) << " ], count: " << p.second<< " , (" << percentage << "%)\n";

		}
		
	}
	*/

	//kompresny pomer
	//int originalbits = totalChars * 8; //lebo je to char (plati len pre ascii kde kazdy znak ma 1 byte)
	int originalbits = origData.size() * 8; // toto ma zmysel len pri utf-8

	int encodedbits = 0;
	for (const auto& a : freq) {
		encodedbits += a.second * codes[a.first].length();
	}
	
	
	double ratio = static_cast<double> (encodedbits) / static_cast<double>(originalbits) *100;
	cout << "\n================================== RATIO AND STUFF===============================================\n" << endl;
	cout << "				  ORIGINAL SIZE: " << originalbits << endl;
	cout << "				   ENCODED SIZE: " << encodedbits << endl;
	cout << "			 COMPRESSION RATIO (ENC/ORIG): " << ratio << " %"<< endl;
	cout << "\n=================================================================================================" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
				 ______________________________________________________________________
				|HUFF FILE | HUFF FILE | HUFF FILE | HUFF FILE | HUFF FILE | HUFF FILE |
				 ----------------------------------------------------------------------
			
*/
	


void writeHuffFile(const std::string& filename, const std::u32string& origText, 
				   const std::string& encodedBits, const std::unordered_map<char32_t, std::string>& codes) {
	
	std::string file = filename + ".huff";
	std::ofstream out(file, std::ios::binary);
	
	if (!out) {
		makeReportFile("writeHuffFile:: nepodarilo sa vytvorit subor na zapis: " + file);
	}

	//MAGIC
	out.write("HUFF", 4);

	//velkost povodneho textu;
	uint64_t size = origText.size(); 
	out.write(reinterpret_cast<const char*>(&size), sizeof(size));

	//platne bity v datach (pocet)
	uint64_t bitSize = encodedBits.size();
	out.write(reinterpret_cast<const char*>(&bitSize), sizeof(bitSize));

	//pocett roznych znakov 
	uint32_t symbolCount = codes.size();
	out.write(reinterpret_cast<const char*>(&symbolCount), sizeof(symbolCount));

	//symbol table
	for (auto ch : codes) {
		uint32_t c;
		uint8_t codeLength;

		c = static_cast <uint32_t>(ch.first);
		codeLength = static_cast<uint8_t> ((ch.second).size());

		out.write(reinterpret_cast<const char*>(&c), sizeof(c));
		out.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));

		//kod ako bajty 
		std::vector<unsigned char> packed = packBits(ch.second);
		out.write(reinterpret_cast<const char*> (packed.data()), packed.size());

	}
	
	//data - zakodovany text
	std::vector<unsigned char> data = packBits(encodedBits);
	out.write(reinterpret_cast<const char*>(data.data()), data.size());


}



template<typename Container , typename Symbol = typename Container::value_type>
Container readHuffFile(const std::string& filename) {

	std::ifstream inFile(filename,std::ios::binary);

	if (!inFile) {
		makeReportFile("readHuffFile:: nepodarilo sa otvorit ");
	}

	// overenie slova HUFF
	std::array<char, 4> magicWord{};
	inFile.read(magicWord.data(), 4);
		//std::memcpy(magicWord,"HUFF",magicWord.size()) != 0
	if (std::string(magicWord.data(), 4) != "HUFF") {
		makeReportFile("readHuffFile: nespravny format suboru neobsahuje 'HUFF'");
	}

	//velkost povodneho textu
	//pozor typy musia sediet s typmi z fcie writeHuffFile
	uint64_t origSize = 0;
	uint64_t bitSize = 0;
	uint32_t symbolCount = 0;

	//pocet povodnych znakov v utf-32
	if (!(inFile.read(reinterpret_cast<char*>(&origSize), sizeof(origSize)))) {
		makeReportFile("readHuffFile: Problem s citanim origSize");
	}
	
	//velkost bitov
	if (!(inFile.read(reinterpret_cast<char*>(&bitSize), sizeof(bitSize)))) {
		makeReportFile("readHuffFile: Problem s citanim bitSize");
	}

	//pocet symbolov 
	if (!(inFile.read(reinterpret_cast<char*>(&symbolCount), sizeof(symbolCount)))) {
		makeReportFile("readHuffFile: Problem s citanim symbolCount");
	}


	std::unordered_map<Symbol, std::string> codes;
	for (auto i = 0; i < symbolCount; i++) {
		uint32_t ch =0;
		uint8_t codeLenght =0;
		
		//nacitanie unicode hodnoty znaku
		if (!(inFile.read(reinterpret_cast<char*>(&ch), sizeof(ch)))) {
			makeReportFile("readHuffFile: chyba pri citani znaku");
		}

		//dlzka jeho kodu 
		if (!(inFile.read(reinterpret_cast<char*>(&codeLenght), sizeof(codeLenght)))) {
			makeReportFile("readHuffFile: chyba pri citani dlzky kodu");
		}
		if (codeLenght == 0) {
			makeReportFile("readHuffFile: nula pri codelength");
		}

		//zistujem kolko bajtov zabere kod 
		size_t byteCount = (codeLenght + 7) / 8; //+7 lebo min pocet bitu aky mozme mat je 1 +7 cize 1 byte 
													//  /8 lebo zistujume kolko bytov je potrebnych ak by vyslo 1.25 
													//   tak uz potrebujeme 2
		
		
		std::vector<unsigned char> packed(byteCount); //packed len nazov NEMYLIT SA 
		if (!(inFile.read(reinterpret_cast<char*> (packed.data()), byteCount/*packed.size()*/))) {
			makeReportFile("readHuffFile: chyba pri citani / a zapisovani do packed vectoru ");
		}
		
		std::string code = unpackBits(packed, codeLenght); //opat dostavame retaze 00 a 11 s presnou velkostou
		codes[static_cast<Symbol>(ch)] = code; //ziskavam povodny huffman kod pre znak

	}

	//nacitanie samotnych dat
	std::vector<unsigned char> data((bitSize+7)/8);
	inFile.read(reinterpret_cast<char*>(data.data()), data.size());

	std::string encodedBits = unpackBits(data, bitSize);
	
	//rekonstrukcia huffman stromu z ulozenych kodov
	std::unique_ptr<links<Symbol>> root = buildTreeFromCodes(codes);

	//dekodovanie do utf-32 a konverzia do utf-8
	//Container decoded = decode2<Container>(encodedBits, root);



	return  decode2<Container>(encodedBits, root);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

			FUNKCIE PRE PRACU S HOCIJAKYMI BINARY SUBORMI (JPG,PDF, MP3)


*/


std::vector<unsigned char> readBinaryFile(const std::string& filename) {

	std::ifstream inputFILE(filename, std::ios::binary);
	if (!(inputFILE.is_open())) {
		makeReportFile("nepodarilo sa otvorit subor ::readBinaryFile");
	}

	//std::vector<unsigned char> out((std::istream_iterator<char>(inputFILE)),std::istream_iterator<char>()); nemozem lebo istream spracuva textovo

	std::vector<unsigned char> out((std::istreambuf_iterator<char>(inputFILE)), std::istreambuf_iterator<char>()); //musim lebo istreambuf iterator priamo spracuva surove bajty co chcem
	return out;
}







////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

			POMOCKA NA NASTAVENIE KONZOLY NA UTF-8 A POTOM SPAT NA DEFAULUT(C-1250/852)

*/







struct ConsoleGuard {

	unsigned int oldInCon;
	unsigned int oldOutCon;

	ConsoleGuard() : oldInCon(GetConsoleCP()) , oldOutCon(GetConsoleOutputCP()) {

		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);

	}
	
	~ConsoleGuard() {
		SetConsoleCP(oldInCon);
		SetConsoleOutputCP(oldOutCon);
	}


};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int spustacia_funkcia2() {


	//	1. nacitanie suboru a konverzia do utf-32
	std::u32string inputFile = readUtf8fileAsu32("input_skuska.txt");

	//	2. vypocet frekvencii a vytvorenie huffman stromu
	auto freq = getFreqGeneric(inputFile);
	std::unique_ptr<links<char32_t>> root = buildTree(freq);

	//	3. generovanie huffman codov
	std::unordered_map<char32_t, std::string> codes;
	GenerateCodes2(root, "", codes);
	
	////////////////// 4. cittanie textu pre vypis

	std::ifstream INput2("input_skuska.txt");
	std::stringstream buff;
	buff << INput2.rdbuf();
	std::string text = buff.str();

	//	5. zakodovanie textu 
	std::string encoded_text = encode2(inputFile, codes);

	////////////// binarne operacie od tadialto ////////////////////
	std::vector<unsigned char> data = packBits(encoded_text);
	writeBinaryFiles("pokus_binar", data);
	
	std::string unpacked_text = unpackBits(data, encoded_text.size()/*velkost v bitoch */);
	/////////////////// po tadialto /////////////////////////

	//	6. ulozenie do .huff suboru 
	writeHuffFile("pokusHUFF", inputFile, encoded_text, codes);

	//	7. citanie a dekodovanie zo suboru 
	std::basic_string<char32_t> decoded_text = readHuffFile<std::u32string>("pokusHUFF.huff");

	std::string dec = u32_to_utf8(decoded_text);


	cout << "=============================== ORIGINAL ==================================================\n" << endl;
	cout << text << endl << endl;
	cout << "=============================== ENCODED ===================================================\n" << endl;
	cout << encoded_text << endl << endl;
	cout << "=================== UNPACKED TEXT FOR COMPARISON WITH ENCODED =============================\n" << endl;
	cout << unpacked_text << endl << endl;
	cout << "=============================== DECODED ===================================================\n" << endl;
	cout << dec << endl << endl;



	std::cout << "======================= Huffman Codes =====================================================\n";
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	for (auto& pair : codes) {
		std::string utf8str = conv.to_bytes(pair.first);
		std::cout << "'" << utf8str << "' : " << pair.second << "\n";
	}
	std::cout << "===========================================================================================\n";
	
	printStatistic(freq, codes,text, true);

	

	cout << "\n=========================================== POKUS S OBRAZKOM(datami) ==================================================\n" << endl;

	std::vector<unsigned char> imageData = readBinaryFile("obrazokPC.bmp");
	
	std::unordered_map<unsigned char, int> imageFreq = getFreqGeneric(imageData);
	
	auto fileRoot = buildTree(imageFreq); //Symbol = unsigned char
	
	std::unordered_map<unsigned char, std::string> imageCodes;
	GenerateCodes2(fileRoot, "", imageCodes);
	



	std::vector<unsigned char> fileEncodedBits = encode2fastThread(imageData, imageCodes);
	
	writeBinaryFiles("imageBinary.bin", fileEncodedBits);

	//citanie
	std::vector<unsigned char> readData = readBinaryFile("imageBinary.bin");
	//std::string bits = unpackBits(readData,fileEncodedBits.size());
	std::vector <unsigned char> decoded = decode2FastThread<std::vector<unsigned char>>(readData, fileRoot, imageData.size());
	
	std::ofstream outFile("obrazok_out.bmp", std::ios::binary);
	outFile.write(reinterpret_cast<const char*> (decoded.data()), decoded.size());
	outFile.close();

	printStatistic(imageFreq, imageCodes, imageData, false);




	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
			# uspora pri mp3 suboroch je velice mala , kedze mp3 subory su uz kompresovane (stratova kompresia)
			# nieco podobne sa deje aj pri png suboroch tiez uz maju kompresiu
	*/
	cout << "\n==================================== POKUS S .mp3 FILE ===============================================\n " << endl;
	
	std::vector<unsigned char> audioData = readBinaryFile("music.mp3");

	std::unordered_map<unsigned char, int> audioFreq = getFreqGeneric(audioData);

	auto musicRoot = buildTree(audioFreq);

	std::unordered_map<unsigned char, std::string> musicCodes;
	GenerateCodes2(musicRoot, "", musicCodes);

	std::string encodedBits2 = encode2(audioData, musicCodes);

	//// od tadialto nepovinne
	//writeBinaryFiles("musicBIN.bin", packBits(encodedBits2)); 
	//std::vector<unsigned char> readMusicData = readBinaryFile("music.mp3");
	//std::string musicBits= unpackBits(readMusicData, encodedBits2.size());
	//// po tadialto 

	std::vector<unsigned char> decodedAudio= decode2<std::vector<unsigned char>>(encodedBits2/*musicBits*/, musicRoot);

	std::ofstream outMusic("music_OUT.mp3", std::ios::binary);
	outMusic.write(reinterpret_cast<const char*>(decodedAudio.data()), decodedAudio.size());
	outMusic.close();

	printStatistic(audioFreq, musicCodes, audioData, false);

	return 0;
}

 




int main()
{
	//spustacia_fcia1();


	//optimalization with UNIQUE PTR
	// na utf-8 konzolu 
	ConsoleGuard Guru;

	//cistenie suboru pre REPORT_MESSAGE.txt
	std::ofstream clear("REPORT_MESSAGE.txt", std::ios::trunc);
	clear.close();

	spustacia_funkcia2();
	

	return 0;
}

