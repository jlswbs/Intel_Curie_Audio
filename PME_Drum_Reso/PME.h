#ifndef NEURONS_H
#define NEURONS_H
 
extern "C" {
  #include <stdint.h>
}

class Neurons
{
	public:				
			
		Neurons();
		int begin();
		void getNeuronsInfo(int* neuronSize, int* neuronsAvailable, int* neuronsCommitted);
		void forget();
		void forget(int Maxif);
		void clearNeurons();
		
		int learn(unsigned char vector[], int length, int category);
		int classify(unsigned char vector[], int length);
		int classify(unsigned char vector[], int length, int* distance, int* category, int* nid);
		int classify(unsigned char vector[], int length, int K, int distance[], int category[], int nid[]);

		void readNeuron(int nid, int* context, unsigned char model[], int* aif, int* category);
		void readNeuron(int nid, unsigned char neuron[]);
		int readNeurons(unsigned char neurons[]);
		int writeNeurons(unsigned char neurons[]);
		int writeNeuron(int nid, unsigned char model[], int category);
				
		//-------------------------------------------
		// debugHelpers, send information
		//-------------------------------------------

		#define DEBUG_STR(txt)           if (_debugEnabled) { Serial.println(txt); }
		#define DEBUG_VAL(label, val)    if (_debugEnabled) { Serial.print(label);  Serial.print(':');  Serial.println(val); }
		#define DEBUG_VALX(label, val)    if (_debugEnabled) { Serial.print(label);  Serial.print(':');  Serial.println(val, HEX); }
		
		void debugEnable(bool enable); 
		void debugNeuronInfo();	
		void debugStatus();	
		
		//-------------------------------------------
		//Functions available in the Geek Library
		//-------------------------------------------

		void setContext(int context, int minif, int maxif);
		void getContext(int* context, int* minif, int* maxif);
		void setRBF();
		void setKNN();
		
		int NCOUNT();
		void NSR(int value);
		int NSR();
		void MINIF(int value);
		int MINIF();
		void MAXIF(int value);
		int MAXIF();
		void GCR(int value);
		int GCR();
		int DIST();
		void CAT(int value);
		int CAT();
		void NID(int value);
		int NID();
		void RSTCHAIN();	
		void AIF(int value);
		int AIF();
		void IDX(int value);									
		
	private:
		bool _debugEnabled=true;	
};

#endif