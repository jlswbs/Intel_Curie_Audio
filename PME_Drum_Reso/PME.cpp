#include "PME.h"

using namespace std;
extern "C" {
  #include <stdint.h>
}

/* PVP */
#define PVP_REGISTER_BASE   (0xB0600000)
#define NM_NCR       		*(volatile int*)(PVP_REGISTER_BASE + 0x00)
#define NM_COMP             *(volatile int*)(PVP_REGISTER_BASE + 0x04)
#define NM_LCOMP            *(volatile int*)(PVP_REGISTER_BASE + 0x08)
#define NM_DIST         	*(volatile int*)(PVP_REGISTER_BASE + 0x0C)
#define NM_IDX        		*(volatile int*)(PVP_REGISTER_BASE + 0x0C)
#define NM_CAT              *(volatile int*)(PVP_REGISTER_BASE + 0x10)
#define NM_AIF              *(volatile int*)(PVP_REGISTER_BASE + 0x14)
#define NM_MINIF            *(volatile int*)(PVP_REGISTER_BASE + 0x18)
#define NM_MAXIF            *(volatile int*)(PVP_REGISTER_BASE + 0x1C)
#define NM_TESTCOMP         *(volatile int*)(PVP_REGISTER_BASE + 0x20)
#define NM_TESTCAT          *(volatile int*)(PVP_REGISTER_BASE + 0x24)
#define NM_NID              *(volatile int*)(PVP_REGISTER_BASE + 0x28)
#define NM_GCR        		*(volatile int*)(PVP_REGISTER_BASE + 0x2C)
#define NM_RSTCHAIN       	*(volatile int*)(PVP_REGISTER_BASE + 0x30)
#define NM_NSR              *(volatile int*)(PVP_REGISTER_BASE + 0x34)
#define NM_FORGET  			*(volatile int*)(PVP_REGISTER_BASE + 0x3C)
#define NM_NCOUNT    		*(volatile int*)(PVP_REGISTER_BASE + 0x3C)


#define NEURONSIZE 	128 //memory capacity of each neuron in byte
#define MAXNEURONS 	128 //number of neurons per chip

// ------------------------------------------------------------ //
//    Constructor to the class Neurons
// ------------------------------------------------------------ 
Neurons::Neurons(){	
}
// --------------------------------------------------------------
//  Initialize the neural network clearing any previous knowledge.
// --------------------------------------------------------------
int Neurons::begin()
{
	NM_FORGET=0;
	if (NM_MINIF==2) return(0); else return(1);
}
// ------------------------------------------------------------ 
// Report the neural network status and capacity
// ------------------------------------------------------------ 
void Neurons::getNeuronsInfo(int* neuronSize, int* neuronsAvailable, int* neuronsCommitted)
{
	*neuronSize=NEURONSIZE;
	*neuronsAvailable=MAXNEURONS;
	*neuronsCommitted=NM_NCOUNT;
}
// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn
// Reset the Maximum Influence Field to default value=0x4000
// ------------------------------------------------------------ 
void Neurons::forget()
{
	NM_FORGET=0;
}
// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// ------------------------------------------------------------ 
void Neurons::forget(int Maxif)
{
	NM_FORGET=0;
	NM_MAXIF=Maxif;
}
// --------------------------------------------------------------
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// Clear the memory of the neurons
// --------------------------------------------------------------
void Neurons::clearNeurons()
{
	int TempNSR=NM_NSR;
	NM_FORGET=0;
	NM_NSR= 16;
	for (int i=0; i< NEURONSIZE; i++) NM_TESTCOMP=0;
	NM_NSR=TempNSR;
}
//-----------------------------------------------
// Learn a vector as belonging to category
// and return the number of committed neurons
//----------------------------------------------
int Neurons::learn(unsigned char vector[], int length, int category)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	NM_CAT=category;
	return(NM_NCOUNT);
}
// ---------------------------------------------------------
// Classify a vector and return its classification status
// NSR=0, unknown
// NSR=8, identified
// NSR=4, uncertain
// ---------------------------------------------------------
int Neurons::classify(unsigned char vector[], int length)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	return(NM_NSR);
}
//----------------------------------------------
// Recognize a vector and return the best match, or the 
// category, distance and identifier of the top firing neuron
//----------------------------------------------
int Neurons::classify(unsigned char vector[], int length, int* distance, int* category, int* nid)
{
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	*distance = NM_DIST;
	*category= NM_CAT & 0x7FFF; //Degenerated bit15 is masked
	*nid =NM_NID;
	return(NM_NSR);
}
//----------------------------------------------
// Recognize a vector and return the response  of up to K top firing neurons
// The response includes the distance, category and identifier of the neuron
// The Degenerated flag of the category is masked
// Return the number of firing neurons or K whichever is smaller
//----------------------------------------------
int Neurons::classify(unsigned char vector[], int length, int K, int distance[], int category[], int nid[])
{
	int responseNbr=0;
	if (length> NEURONSIZE) length=NEURONSIZE;
	for (int i=0; i<length-1; i++) NM_COMP=vector[i];
	NM_LCOMP=vector[length-1];
	for (int i=0; i<K; i++)
	{
		distance[i] = NM_DIST;
		if (distance[i]==0xFFFF)
		{ 
			category[i]=0xFFFF;
			nid[i]=0xFFFF;
		}
		else
		{
			responseNbr++;
			category[i]= NM_CAT & 0x7FFF; //Degenerated bit15 is masked
			nid[i] = NM_NID;
		}
	}
return(responseNbr);
}
//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
//-------------------------------------------------------------
void Neurons::readNeuron(int nid, int* context, unsigned char model[], int* aif, int* category)
{
	if (nid==0) return;
	int TempNSR=NM_NSR;
	NM_NSR=0x0010;
	NM_RSTCHAIN= 0;
	for (int i=0; i< nid-1; i++) NM_CAT; // move to index in the chain of neurons
	*context=NM_NCR;
	for (int j=0; j<NEURONSIZE; j++) model[j]=NM_COMP;
	*aif=NM_AIF;
	*category=NM_CAT;
	NM_NSR=TempNSR; // set the NN back to its calling status
}
//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting index is 1
// Returns an array of 264 bytes with the following format
// 2-bytes NCR, NEURONSIZE-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//-------------------------------------------------------------
void Neurons::readNeuron(int nid, unsigned char neuron[])
{
	if (nid==0) return;
	int TempNSR=NM_NSR; // save value to restore NN status upon exit of the function
	NM_NSR=0x0010;
	NM_RSTCHAIN= 0;
	for (int i=0; i< nid-1; i++) NM_CAT; // move to index in the chain of neurons
	int Temp=NM_NCR;
	neuron[0]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[1]=(unsigned char)(Temp & 0x00FF);
	for (int j=0; j<NEURONSIZE; j++) { Temp=NM_COMP; neuron[j+2]= (unsigned char)Temp;}
	Temp=NM_AIF;
	neuron[NEURONSIZE+2]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+3]=(unsigned char)(Temp & 0x00FF);
	Temp=NM_MINIF;
	neuron[NEURONSIZE+4]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+5]=(unsigned char)(Temp & 0x00FF);
	Temp=NM_CAT;
	neuron[NEURONSIZE+6]=(unsigned char)((Temp & 0xFF00)>>8);
	neuron[NEURONSIZE+7]=(unsigned char)(Temp & 0x00FF);
	NM_NSR=TempNSR; // set the NN back to its calling status
}
//----------------------------------------------------------------------------
// Read the contents of the committed neurons
// The neurons array should be sized to HEADERSIZE + MAXNEURON * (NEURONSIZE + 8)
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//----------------------------------------------------------------------------
int Neurons::readNeurons(unsigned char neurons[])
{
	int ncount= NM_NCOUNT;
	int TempNSR= NM_NSR; // save value to restore NN status upon exit of the function
	NM_NSR= 0x0010;
	NM_RSTCHAIN= 0;
	int offset=8;
	int MaxLength=NEURONSIZE; //can be less than NEURONSIZE
	int recLen=MaxLength+8; // memory plus 8 bytes of register data
	neurons[0]= offset; // default header size of 8 bytes
	neurons[1]= 0; // reserved to encode a future format identifier
	neurons[2]= (MaxLength & 0xFF00)>>8;
	neurons[3]= (MaxLength & 0x00FF);
	neurons[4]= (ncount & 0xFF000000)>>24;
	neurons[5]= (ncount & 0x00FF0000)>>16;
	neurons[6]= (ncount & 0x0000FF00)>>8;
	neurons[7]= (ncount & 0x000000FF);
	int Temp;	
	for (int i=0; i< ncount; i++)
	{
		offset+=(i*recLen);
		Temp=NM_NCR;
		neurons[ offset]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + 1]=(unsigned char)(Temp & 0x00FF);
		for (int j=0; j< MaxLength; j++) 
		{
			Temp=NM_COMP;
			neurons[ offset + 2 + j]=(unsigned char)(Temp & 0x00FF);
		}
		Temp=NM_AIF;
		neurons[ offset + MaxLength + 2]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 3]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_MINIF;
		neurons[ offset + MaxLength + 4]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 5]=(unsigned char)(Temp & 0x00FF);
		Temp=NM_CAT;
		neurons[ offset + MaxLength + 6]=(unsigned char)((Temp & 0xFF00)>>8);
		neurons[ offset + MaxLength + 7]=(unsigned char)(Temp & 0x00FF);
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	return(ncount);
}
//---------------------------------------------------------------------
// Clear the committed neurons and restore a new content for the neurons
// from an input array formatted as follows
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//---------------------------------------------------------------------
int Neurons::writeNeurons(unsigned char neurons[])
{
	int offset= neurons[0];
	int MaxLength= (neurons[2]*256) + neurons[3];
	int ncount= (neurons[4]<<24) + (neurons[5]<<16) + (neurons[6]<<8) + neurons[7];
	if (ncount>MAXNEURONS) ncount=MAXNEURONS;
	int recLen=MaxLength+8;	
	int TempNSR=NM_NSR; // save value to restore NN status upon exit of the function
	int TempGCR=NM_GCR;
	NM_FORGET= 0;
	NM_NSR= 0x0010;
	NM_RSTCHAIN=0 ;
	for (int i=0; i< ncount; i++)
	{
		offset +=(i*recLen);
		NM_NCR= neurons[offset+1];
		for (int j=0; j<MaxLength; j++) NM_COMP=neurons[offset+2+j];
		NM_AIF= (neurons[offset + MaxLength +2]<<8)+neurons[offset + MaxLength+3];
		NM_MINIF= (neurons[offset + MaxLength+4]<<8)+neurons[offset + MaxLength+5];	
		NM_CAT= (neurons[offset + MaxLength+6]<<8)+neurons[offset + MaxLength+7];
	}
	NM_NSR=TempNSR; // set the NN back to its calling status
	NM_GCR=TempGCR;
	return(ncount);
}
				
//-------------------------------------------
// debugHelpers, send information
//-------------------------------------------

// enable debugging information on Serial port.
void Neurons::debugEnable(bool enable) {
  _debugEnabled = enable;
} // debugEnable()

void Neurons::debugNeuronInfo() {
	// tdb.
} // debugNeuronInfo()

void Neurons::debugStatus() {
  // no output.
} // debugStatus

/**********************************************************************/
/* 
 * The following functions are supplied in the Advanced library
 */
/**********************************************************************/

// ------------------------------------------------------------ 
// Set a context and associated minimum and maximum influence fields
// ------------------------------------------------------------ 
void Neurons::setContext(int context, int minif, int maxif)
{
	NM_GCR=context;
	NM_MINIF=minif;
	NM_MAXIF=maxif;
}
// ------------------------------------------------------------ 
// Get a context and associated minimum and maximum influence fields
// ------------------------------------------------------------ 
void Neurons::getContext(int* context, int* minif, int* maxif)
{
	*context = NM_GCR;
	*minif= NM_MINIF; 
	*maxif =NM_MAXIF;
}
// --------------------------------------------------------
// Set the neurons in Radial Basis Function mode (default)
//---------------------------------------------------------
void Neurons::setRBF()
{
	int tempNSR=NM_NSR;
	NM_NSR=tempNSR & 0xDF;
}
// --------------------------------------------------------
// Set the neurons in K-Nearest Neighbor mode
//---------------------------------------------------------
void Neurons::setKNN()
{
	int tempNSR=NM_NSR;
	NM_NSR=tempNSR | 0x20;
}
// --------------------------------------------------------
// Read the number of committed neurons
//---------------------------------------------------------
int Neurons::NCOUNT()
{
	return(NM_NCOUNT);
}
// --------------------------------------------------------
// Get/Set the Network Status register
// bit 2 = UNC (read only)
// bit 3 = ID (read only)
// bit 4 = SR mode
// bit 5= KNN mode
//---------------------------------------------------------
void Neurons::NSR(int value)
{
	NM_NSR=value;
}
int Neurons::NSR()
{
	return(NM_NSR);
}
// --------------------------------------------------------
// Get/Set the Minimum Influence Field register
//---------------------------------------------------------
void Neurons::MINIF(int value)
{
	NM_MINIF=value;
}
int Neurons::MINIF()
{
	return(NM_MINIF);
}
// --------------------------------------------------------
// Get/Set the Maximum Influence Field register
//---------------------------------------------------------
void Neurons::MAXIF(int value)
{
	NM_MAXIF=value;
}
int Neurons::MAXIF()
{
	return(NM_MAXIF);
}
// --------------------------------------------------------
// Get/Set the Global Context register
//---------------------------------------------------------
void Neurons::GCR(int value)
{
	NM_GCR=value;
}
int Neurons::GCR()
{
	return(NM_GCR);
}
// --------------------------------------------------------
// Get the Distance register
//---------------------------------------------------------
int Neurons::DIST()
{
	return(NM_DIST);
}
// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void Neurons::CAT(int value)
{
	NM_CAT=value;
}
int Neurons::CAT()
{
	return(NM_CAT);
}
// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void Neurons::NID(int value)
{
	NM_NID=value;
}
int Neurons::NID()
{
	return(NM_NID);
}
// --------------------------------------------------------
// Reset the neuron chain to fisrt
//---------------------------------------------------------
void Neurons::RSTCHAIN(void)
{
	NM_RSTCHAIN=0;
}
// --------------------------------------------------------
// Get/Set the Active Influence Field register
//---------------------------------------------------------
void Neurons::AIF(int value)
{
	NM_AIF=value;
}
int Neurons::AIF()
{
	return(NM_AIF);
}
// --------------------------------------------------------
// Set the Component Index register
//---------------------------------------------------------
void Neurons::IDX(int value)
{
	NM_IDX=value;
}

// Přepiš obsah jednoho neuronu (nid) novým modelem
int Neurons::writeNeuron(int nid, unsigned char model[], int category) {
    if (nid <= 0) return 0;

    int TempNSR = NM_NSR;   // uložit status
    NM_NSR = 0x0010;        // režim zápisu
    NM_RSTCHAIN = 0;

    // posuň se na správný neuron v řetězci
    for (int i = 0; i < nid - 1; i++) NM_CAT;

    // zapiš komponenty
    for (int j = 0; j < NEURONSIZE-1; j++) NM_COMP = model[j];
    NM_LCOMP = model[NEURONSIZE-1];

    // nastav kategorii
    NM_CAT = category;

    NM_NSR = TempNSR;       // obnov status
    return nid;
}


