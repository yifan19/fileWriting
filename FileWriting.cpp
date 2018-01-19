//////////////////////////////////////////////////////////////
//
// Your #includes here; make sure you are allowed them ...
//

#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <limits.h>

using namespace std;
//////////////////////////////////////////////////////////////
//
// local #includes and function declarations; do not remove
//

#ifndef MARMOSET_TESTING


#include <iostream>



struct Student{
	int studentID;
	int grade;
};

struct Dataset{
	int numStudents;
	Student* students;
};

struct Rejects{
	int numRejects;
	int* rejects;
};

struct Mode{
	int numModes;
	int* modes;
};

struct Statistics{
	int   minimum;
	float average;
	int   maximum;
	float popStdDev;
	float smplStdDev;
	Mode  mode;
	int   histogram[10];
};

#endif

//////////////////////////////////////////////////////////////
//
// struct definitions
//


//////////////////////////////////////////////////////////////
//
// Function declarations; do not remove
//
bool sort(int dataset[], const int size);
bool stringToInt(const char input[], int& value);
void merge (int data[], const int lower, const int upper);
char* stringConcat(const char base[] , const char exten[]);

char* stringCut(const char base[]);

bool statistics(const float dataset[], const int size,
                float& min, float& avg, float& max,
	        float& popSD, float& smplSD, float& mdn);

int minimum(const int dataset[], const int size);

float average(const int dataset[], const int size);

int maximum(const int dataset[], const int size);

float popStdDev(const int dataset[], const int size);

float smplStdDev(const int dataset[], const int size);

int mode(const int dataset[], const int size, Mode& modeStructure);

int modHelper(const int data[], const int size,int mode[] ,int currentIndex,
		int currentRep, int maxRep, int indOfMod);

int computeStatistics(Dataset& data, Statistics& stats);

int readCSV(const char filename[],const int minAcceptableID,
				const int maxAcceptableID,Dataset& data,
				Rejects& rejects);
				
int writeCSV(const char filename[], const Statistics& stats);

int readSCVHelp(char inputLine [],int& lineNum,Rejects& rejectionStruct,
				const int minAcceptableID, const int maxAcceptableID, Dataset& dataStruct);


//////////////////////////////////////////////////////////////
//
// Your code here ...
//


// SORT OR HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////
bool sort(int dataset[], const int size){
	merge(dataset,0,size);
	return true;
}

void merge (int data[], const int lower, const int upper){ // [lower,upper)

int middlePoint = (upper+lower)/2;
  if ( (upper-lower) > 2){ //keep dividing
    merge(data,lower,middlePoint);
    merge(data,middlePoint,upper);
	}
  
  int temp[upper-lower]; // temp array
  int i= lower; //starting point of i
  int j= middlePoint; //starting point of j
  int k = 0; //new array counter
  
  while( (i< middlePoint) && (j < upper) ){ //while one of them is not empty
      
    if(data[i] <= data[j]){
     
      temp[k] = data[i];
      i++;
    }
  
    else{
      temp[k]= data[j];
      j++;
    }
    k++; //next array
        
  }
  // if one of the data array is depleted before the other one
  while (j < upper){
    
    temp[k] = data[j];
    j++;
    k++;
  }
  
  while(i < middlePoint){
    
    temp[k] = data[i];
    i++;
    k++;
  }
  
  //copy it back to data
  for(int c = 0; c < (upper-lower); c++){
    
    data[lower+c] = temp[c];
  }
  
}

bool stringToInt(const char input[], int& value) {
  enum state{ gotSign, gotNum, Done, Reject, Start};
  state conversion = Start;
  if (input[0] == 0 ){
		//cout << "rejection" << endl;
	  return false;
	}
  
  int i = 0;
  int num = 0;
  int sign = 1; //pos by default
  
  while (input[i] != 0){	 //is the input nan
    switch(conversion){
      case Start:
        if ( (input[i] == '-') || (input[i] == '+') )
          conversion = gotSign;
        else if ( (input[i] >= '0') &&  (input[i] <= '9') )
          conversion = gotNum;
        else{
          conversion = Reject;
        }
        break;
        
      case gotSign:
        if ( input[i] == '+'){
          sign = 1;
          conversion = gotNum;
          i++;
        } else{
          sign = -1;
          conversion = gotNum;
          i++;
        }
        break;
				
      case gotNum:
				if ( (input[i] >= '0') &&  (input[i] <= '9') &&
						num <= (INT_MAX -input[i] + 48)/10){ //assume its a positive number
					num *= 10; //multiply the previous number by 10
          num += (input[i] - 48); //ascii value of 0 is 48.
					i++;
				} else {
					conversion = Reject;
				}
        break;
				
      case Reject:
				if ( num == 214748364 && sign == -1 ){
					if (input[i] == '8' && input[i+1] == 0){
					value = INT_MIN;
					return true;
					}
				}
				return false;
        break;
    } // switch loop
    
  }//while loop
	value = sign * num;
	return true;
}

char* stringConcat(const char base[] , const char exten[]){
	
	if (base == NULL){
		return NULL;
	}
    
    char* myString = new char[40];
    
    int index = 0; //count how many index values we are at
    
    for (;base[index] != 0; index++){ //copy base
        
        myString[index] = base[index];
        //cout << myString[index] << endl;
        
    }
    // now index is pointing to an empty area due to the for loop running one more time
    // abc  .txt
    for(int i = 0; exten[i] != 0; i++){ // copy
        myString[index] = exten[i];
        index++;
    }
    myString[index+1] = 0;
    return myString;
}

char* stringCut(const char base[]){
    // cut out the .ext
	if (base == NULL){
		return NULL;
	}
    char* cutString = new char[40];
    int i = 0;
    for (; base[i] != 0; i++){
        if (base[i] == '.'){
            cutString[i]= 0;
        } else {
            cutString[i] = base[i];
        }
    }
    cutString[i] = 0;
    return cutString;
}

//STATISTICS
/////////////////////////////////////////////////////////////////////

int minimum(const int dataset[], const int size) {

	int c = 1 ; //starting at index value 1 since 0 is already done
	int minVal = dataset[0];
	while(c < size){
		
		if (dataset[c] < minVal){
			minVal = dataset[c];
		}
		
		c++ ;
	}
	
	return minVal;
}

float average(const int dataset[], const int size) {

	int c = 0; //counter
	float total = 0;
	while (c < size){
		total += dataset[c];
		c++;
	}
	
	return (total/size);
}

int maximum(const int dataset[], const int size) {
	
	int c = 1 ; //starting at index value 1 since 0 is already done
	int maxVal = dataset[0];
	while(c < size){
		
		if (dataset[c] > maxVal){
			maxVal = dataset[c];
		}
		
		c++ ;
	}
	
	return maxVal;
}

float popStdDev(const int dataset[], const int size) {
	int x = 0; //counter
	float variance = 0;
	float av = average(dataset, size);
	
	while (x < size){
		variance += (dataset[x]-av)*(dataset[x]-av);
		//cout << variance << endl;
		x++;
	}
	
	return sqrt(variance/size);
	
}
float smplStdDev(const int dataset[], const int size) {
	
	int x = 0; //counter
	float variance = 0;
	float av = average(dataset, size);
	
	while (x < size){
		variance += (dataset[x]-av)*(dataset[x]-av);
		//cout << variance << endl;
		x++;
	}
	
	
	return sqrt(variance/(size-1));
}


void histogram(const int dataset[], const int size, int histogram[]){
    
    int bucket;
    
    for (int i = 0; i < 10; i++){
        histogram[i] = 0;
    }
    // initialize histogram to 0
    
    for (int i = 0; i < size; i++){
        bucket = dataset[i] / 10;
        if (bucket >= 10){ //100%
            bucket--;
        }
        
        histogram[bucket]++;
        
    }
    
}

int mode(const int dataset[], const int size, Mode& modeStruct){
	
	modeStruct.modes = new int[size];
	int*& newNameforModeArray = modeStruct.modes;
	// declare the mode array as another name
	int copyDataset[size];
	for(int i = 0; i < size; i++){
		copyDataset[i] = dataset[i];
	}
	sort(copyDataset,size);
	
	if (size == 1){
		newNameforModeArray[0] = dataset[0];
		modeStruct.numModes = 1;
		return 0;
	}
// 	struct Mode{
// 	int numModes;
// 	int* modes;
// };

	
	
	modeStruct.numModes = modHelper(copyDataset,size,newNameforModeArray,1,1,1,0);
	//cout << modeStruct.numModes << endl;
	return 0;
}

int modHelper(const int data[], const int size,int mode[] ,int currentIndex,
		int currentRep, int maxRep, int indOfMod){ //max rep and current start at 1 num Of Mod starts at 0
	
	if (currentIndex > (size-1)){ // base case
		return (indOfMod+1); // since it is an index
	}
	
	
	if (data[currentIndex] != data[currentIndex -1]){ // one number is not gonna break the record
		
		
		if ((currentRep == maxRep) && (maxRep == 1)){//technically everyone is a mod when the max is 1
		
			if (currentIndex == 1){ //to start off the first number
				mode[0] = data[0];
			}
			indOfMod++; //about the make an other entry
			mode[indOfMod] = data[currentIndex]; // set number as the next mod
			//reset the currentRep only
			return modHelper(data,size,mode,currentIndex+1, 1, maxRep, indOfMod);
		}
		else{ //rejected number because below record
			return modHelper(data,size,mode,currentIndex+1, 1, maxRep,indOfMod);
		}
	}
	
	else{//if the number is equal to the previous
		currentRep++; //repetition starts  ONLY INCREMENT IT HERE
	}
		
		
		
		
		
	if (currentRep > maxRep){ //beating the record (might continue to beat the record)
		mode[0] = data[currentIndex]; // set it as the new champ indOfMod = 0 rep++
		return modHelper(data,size,mode, currentIndex+1, currentRep, currentRep,0);
		// it will reset if the record is beaten again.
	}
	else if (currentRep == maxRep){ //just equalized the record
		indOfMod++;
		mode[indOfMod] = data[currentIndex];
		//
		return modHelper(data,size,mode,currentIndex+1, currentRep, maxRep,indOfMod);
		
	}
	else{//not record breaking... nothing interesting
		return modHelper(data,size,mode, currentIndex+1, currentRep, maxRep, indOfMod);
	}
	
}

int computeStatistics(Dataset& data, Statistics& stats){
	
	
    int size = data.numStudents;
    
    if(size <= 0){ //error check
		return -1;
	}
    
    int gradeArray[size];
    for (int i = 0; i < size; i++){
        gradeArray[i] = data.students[i].grade;
        }
	stats.maximum = maximum(gradeArray,size);
	stats.minimum = minimum(gradeArray,size);
	stats.average = average(gradeArray,size);
	
	stats.smplStdDev = smplStdDev(gradeArray,size);
	stats.popStdDev = popStdDev(gradeArray,size);
	
	histogram(gradeArray,size,stats.histogram);
	mode(gradeArray,size,stats.mode);
	

	return 0;
}



//READ
///////////////////////////////////////////////////////////////////
int readSCVHelp(char inputLine [],int& lineNum,Rejects& rejectionStruct,
				const int minAcceptableID, const int maxAcceptableID, Dataset& dataStruct){
	enum readingState {WSbID,WSaID,WSbGrade,WSaGrade,commas,ID,SCORE,ERROR,DONE,SKIP};
	
	const int maxNumErrors = 10; //max numbers of error
	int i = 0; //counter
	char num[20];
	int numInd = 0;
	char grade[20];
	char gradeInd= 0;
	int myNum;
	int myGrade;
	int fnRes; // function result
    	
	readingState cancerInput = WSbID; //cancerInput is my state :)
	
	//start with WhiteSpaces;
	if (inputLine[0] == 0){  //if empty next line skip...
			cancerInput = SKIP;
		}
	
	while (inputLine[i] != 0 ){
		
		switch (cancerInput){
			case WSbID: //if blank line I error it.
				if ( (inputLine[i] == 9) || (inputLine[i] == ' ') ){ // 9= tab
					i++;//next
				}
				else if ( (inputLine[i] >= '0') && (inputLine[i] <= '9') ){
					cancerInput = ID;
				
				}
				else if ((inputLine[i] < 0)){
					i++; //a weird error im having so move on
				
				}
				
				else if (inputLine[i+1] ==0){ //if the next char is 0; this is a blank space line
				    cancerInput = SKIP;
				}
				
				else{
					
					cancerInput = ERROR;
					
					
				}
				break;
			
			case WSaID:
				if ( (inputLine[i] == 9) || (inputLine[i] == ' ') ){ // 9= tab
					i++; //next
				}
				else if ( inputLine[i] == ',' ){
					cancerInput = commas;
					//cout << "commas detected" << endl;
				
				} else {
				
					cancerInput = ERROR;
					
				}
				break;
				
			case WSbGrade:
				if ( (inputLine[i] == 9) || (inputLine[i] == ' ') ){ // 9= tab
					i++;//next
				}
				else if (( (inputLine[i] >= '0') && (inputLine[i] <= '9') ) || (inputLine[i] == '-') ){
					
					//cout << "grade detected..." << endl;
					cancerInput = SCORE;
				
				} else {
					
					cancerInput = ERROR;
					
				}
				
				break;
			case WSaGrade:
			
				if (inputLine[i+1] == 0){
					cancerInput = DONE; //it is done!
					inputLine[i] == 0; //break
					
				}
				//still want to check
				if ( (inputLine[i] == 9) || (inputLine[i] == ' ') || (inputLine[i] == '\r' ) ){ // 9= tab
					
					
					i++;//next
					
				}
				
				// else if ( (inputLine[i] == ',') || ( (inputLine[i] >='0') && (inputLine[i] <='9') )  ){
					// cancerInput = ERROR;
					//will run to error again next time since no i++;
					
				// }
				
				else{
					cancerInput = ERROR;
					
				}
				break;
			case commas:
				if (inputLine[i] == ','){
					cancerInput = WSbGrade;
					i++;
					//cout << "commas case reached" << endl;
				} else{
					
					cancerInput = ERROR;
				}
				break;
				
			case ID:
				if (( (inputLine[i] >= '0') && (inputLine[i] <= '9') ) || (inputLine[i] == '-') ){
					//cout << "number detected: " << inputLine[i] << endl;
					num[numInd] = inputLine[i];
					numInd++;
					i++;
					
					if (numInd > 20){
						cancerInput = ERROR;
					}
					
					if (inputLine[i] == 0){
					    cancerInput = ERROR;
					}
				
				}
				else if ((inputLine[i] == 9) || (inputLine[i] == ' ') || (inputLine[i] == ',') ){
					//cout << "end of the ID reading..." << in << endl;
					//asci to integer;
					//so gimme that null terminator
					num[numInd] = 0;
					fnRes = stringToInt(num,myNum);
					//if false exit
					if (fnRes == false)
					{
						cancerInput = ERROR; //maybe just reject it
						//return -1;
					}
					
					//cout << "Id..." << myNum << endl;
					//------------------------------------//
					if ( (myNum > maxAcceptableID) || (myNum < minAcceptableID) ){
						//num out of bound
						cancerInput = ERROR;
					} else{
						cancerInput = WSaID;
					}
				}
				// else if(in == ','){
					// cancerInput = commas;
				else{ // if nothing goes
					
					cancerInput = ERROR;
				}
				break;
			case SCORE:
				if (( (inputLine[i] >= '0') && (inputLine[i] <= '9') ) || (inputLine[i] == '-') ){
					//cout << "grade detected: " << inputLine[i] << endl;
					grade[gradeInd] = inputLine[i];
					gradeInd++;
					
					
					if (gradeInd > 5){
						cancerInput = ERROR;
						inputLine[i] = 100;
					}
					//if too many numbers
					i++;
					//char next = inputLine[i];
					//cout << "next char   " <<(int)next << endl;
					
					if (inputLine[i] == 0){ //already over
						grade[gradeInd] = 0;
						fnRes = stringToInt(grade,myGrade);
						if (fnRes == false){
							
							cancerInput = ERROR;
							inputLine[i] = 100;
							//return -1;
						}
						//cout << "mygrade saved is" << myGrade << endl;
						//------------------------------------//
						if ( (myGrade > 100) || (myGrade < 0) ){
						//num out of bound
						
						    cancerInput = ERROR;
						    inputLine[i] = 100; // make sure the error goes thru
						} else{
						    cancerInput = DONE;
						}
					}
				}
				else if ((inputLine[i] == 9) || (inputLine[i] == ' ') || (inputLine[i] == '\r') ){
					//NASTY BUG here 98,
					//asci to integer
					grade[gradeInd] = 0; //null terminationbb
					
					
					fnRes = stringToInt(grade,myGrade);
					
					if (fnRes == false){
						
						cancerInput == ERROR;
						//return -1
					}
					//cout << "mygrade is" << myGrade << endl;
					//------------------------------------//
					else if ( (myGrade > 100) || (myGrade < 0) ){
							// num out of bound
							cancerInput = ERROR;
					} else{
							cancerInput = WSaGrade;
					}
					
					
					
				}
				else{ // if nothing goes
			
					cancerInput = ERROR;
					//return -1;
				}
				
				break;
			case ERROR:
				//cout << "ERROR LINE!!!!!s " << lineNum << endl;
				rejectionStruct. rejects[rejectionStruct.numRejects]= lineNum;
				rejectionStruct. numRejects++;
				inputLine[i] = 0;//break
				break;
				
			case SKIP:
			    lineNum--;
			    return 0;
			    break;
		}//switch loop
	}//whileloop
	if ( cancerInput == DONE){
		
		dataStruct.students[dataStruct.numStudents].studentID = myNum;  //fill student struc
		dataStruct.students[dataStruct.numStudents].grade= myGrade;
		dataStruct.numStudents++; // add one student;
		
	}
	else if ((cancerInput != ERROR) && (cancerInput != SKIP)){
		rejectionStruct. rejects[rejectionStruct.numRejects]= lineNum;
		rejectionStruct. numRejects++;
	}
		
	return 0;
	
}

int readCSV(const char filename[],const int minAcceptableID,
				const int maxAcceptableID,Dataset& data,
				Rejects& rejects) {
	if (filename == NULL){
		return -1;
	}
	char* betterName;
	bool extDetected = false; //abc
	
	for (int i = 0; filename[i] != 0 ; i++){
		if (filename[i] == '.'){
			extDetected = true; //abc.csv
		}
	}
	
	///////////////////////////////////////////
		betterName = stringConcat(filename,".csv");
	/////////////////////////////////////////////////////////////////////
	
	const int maxLineLength = 500;
	char line[maxLineLength];
	ifstream infile;
	// declare the file object
	//cout << betterName << endl;
	for (int i = 0; i < 20 ; i++){
		//cout << betterName[i] << endl;
	}
	if(!extDetected){
		infile.open(betterName);
	} else{
	    
		infile.open(filename);
	}
	// open the file
	if (!infile.is_open()) //if the file isnt open
		return -2;
	// Unable to open filebool done = false;
	
	int fileLineNumber = 1;
	bool imDone = false; //not Done yet
	const int maxNumErrors = 9; //max numbers of error
	
	int res; // res for the result of helper function
	

	rejects. rejects = new int[10]; //reject array maximum 10 give one more for error purposes
	rejects. numRejects = 0;
	data. numStudents = 0;
	data. students = new Student[50];
	
	
	
	while (!imDone) {
		if (minAcceptableID > maxAcceptableID){
				data.numStudents = 0;
				data. students = NULL;
				return -1;
				//error checking
			}
			if (rejects.numRejects > maxNumErrors-1){
				data.numStudents = 0;
				data. students = NULL;
				return -1;
				// if more than
			}//error checking again
			  
		//cout << rejects.numRejects << endl;
			
		// Read from file++fileLineNumber;

		if (!infile.getline(line, maxLineLength)) {
			// Get next line in my array
			//will return false if can't store it
			
			if (infile.eof()) {
				imDone = true; //exit on the if statement
			//End of filedone = true;
			}else {
			// Some error reading file
				data.numStudents = 0;
				data. students = NULL;
				return -1;
			}
		}
		if (!imDone){
			//cout << "processing.. " << line << endl;
			
			
	
			res =readSCVHelp(line,fileLineNumber,rejects, minAcceptableID,maxAcceptableID, data);
			if(res == -1){
			data.numStudents = 0;
			data. students = NULL;
			return -1;
				
				// error have to exit
			}
			
			//increment lineCount
			fileLineNumber++;
			
			 
		}//if loop
		
	}//while
	
    infile.close();
	
	if (rejects.numRejects == 0){
		return 0;
	} else{
		return 1;
	}
}





//WRITE
///////////////////////////////////////////////////////////

int writeCSV(const char filename[], const Statistics& stats){
    
    char* betterName = stringCut(filename);
    betterName = stringConcat(betterName,".stat");
	//cout << betterName << endl;
	ofstream output;
	
	output.open(betterName);
	// }
	
	
	if(!output.is_open()){
	    return -1;
	}
	
	output << "Minimum: " << stats.minimum << "\n";
	output << "Average: "<< stats.average << "\n";
	output << "Maximum: "<< stats.maximum << "\n";
	output << "Population Standard Deviation: " << stats.popStdDev<< "\n";
	output << "Sample Standard Deviation: "<< stats.smplStdDev<< "\n";
	output << "Modes: ";
		
	for (int i = 0; i < stats.mode.numModes; i++){
	    output << stats.mode.modes[i];
	    if( (i+1) < stats.mode.numModes ){
	        output << ", ";
	    }
	  
	}
	output << "\n";
	
	output << "Histogram:" << "\n";
	
	for (int i = 0; i < 10; i++){
	    output << "[" << i*10 << "-" << ( (i+1)* 10 )-1 + i/9 << "]: " ;
	    output << stats.histogram[i]<< "\n";
	}
	
	
	
	output.close();
	
	
	return 0;
}
//////////////////////////////////////////////////////////////
//
// Test Driver
//
// Do not remove the #ifndef and its associated #endif
//

#ifndef MARMOSET_TESTING

#define isNaN(X) (X != X)  // NaN is the only float that is not equal to itself

int main(const int argc, const char* const argv[]) {
  	
	// Some test driver code here ....
	int minAcceptableID = 22200000;
	int maxAcceptableID = 22299999;

	Dataset data = {0};
	Rejects rejects = {0};
	Statistics stats = {0};
	

	std::cout << std::endl << "Implement some more appropriate tests in main()" << std::endl << std::endl;

	if(readCSV("inputfile.csv", minAcceptableID, maxAcceptableID, data, rejects) < 0)
	{
		std::cout << ".csv file could not be read" << std::endl;
	}

	if (computeStatistics(data, stats) < 0)
	{
		std::cout << "Stats could not be computed" << std::endl;
	}

	if (writeCSV("inputfile", stats) < 0)
	{
		std::cout << ".stat file could not be written" << std::endl;
	}
	
	std::cout << "Minimum: " << stats.minimum << std::endl;
	std::cout << "Average: " << stats.average << std::endl;
	
	std::cout << "Maximum: " << stats.maximum << std::endl;
	std::cout << "Population standard deviation: " << stats.popStdDev << std::endl;
	std::cout << "Sample standard deviation: " << stats.smplStdDev << std::endl;
	
	
	for (unsigned char i = 0; i < stats.mode.numModes; i++)
	{
		std::cout << "Mode: " << stats.mode.modes[i] << std::endl;
	}

	for (unsigned char i = 0; i < 10; i++)
	{
		std::cout << "Histogram bin [" << (i*10) << ", " << ((((i+1)*10) - 1) + i/9) << "]: " << stats.histogram[i] << std::endl;
	}
	
	
	// struct Student{
	// int studentID;
	// int grade;
// };

// struct Dataset{
	// int numStudents;
	// Student* students;
// };

	for(int i = 0; i < data.numStudents;i++){
		cout << data.students[i].studentID << endl;
		cout << data.students[i].grade << endl;
	}
	
	for(int i = 0 ; i < rejects.numRejects; i++){
		cout << rejects.rejects[i] << endl;
	}
	
	return 0;
}

#endif
