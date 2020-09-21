#include <FHE.h>
#include <EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

#include <iostream>
#include <stack>
#include <string>
#include <stdlib.h>

using namespace std;
//stack<int> theStack;
stack<Ctxt> theStack;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;
//ZZX *x1 = &G;

void setupHELib();
bool isOp(string token);
void evaluate(char op);

void greeting() {
        int i;
	cout << "Welcome to the homomorphic encryption calculator" << endl;
//	cout << "Enter expressions in reverse polish notation" << endl;

//	cout << "Enter q to quit" << endl;
}
/*template <typename T, std::size_t N>
vector<int*> init_vector(T (&func)[N])
{
    std::vector<int*> vec;
    for (std::size_t i = 0; i < N; ++i)
    {
        vec.push_back(&func[i]);
    }
    return vec;
}
*/
int main(int argc, char** argv) {
	// Local variables
	string token;
	greeting();

	// setup FHE
    setupHELib();
    EncryptedArray ea(*context, G); 
   
	// Main loop
	while(true) {
		
		cin >> token;
		if(token[0] == 'q') {//If option is quit
			break;
		}
		else if(isOp(token)) {
            if (theStack.size() < 2) { //If there are not atleast two numbers
                cout << "not enough numbers on the stack" << endl;
            }
            else {
			    // perform the operation
			    evaluate(token[0]);
            }
		}
		else {
			// should be a number, push it
			// encrypt it first 
       // std::vector<long*> vec;
        //vec.push_back(atol(token.data()));
        //func[j] = atoi(token.data());
	//const vector<int*> vec = init_vector(func);
            Ctxt& c = *(new Ctxt(*publicKey));
            NewPlaintextArray p(ea); 
            //p.encode(atol(token.data()));
            encode(ea, p, atoi(token.data()));
            ea.encrypt(c, *publicKey, p); 
            
			//theStack.push(atoi( token.data() ));
            theStack.push(c);
		}

	}
	// TODO: decrypt before printing
    NewPlaintextArray p(ea);
    ea.decrypt(theStack.top(), *secretKey, p);
	cout << "The answer is: " ;
    p.print(cout);
    cout << endl;
}

bool isOp(string token) {
	return (token[0] == '+' || token[0] == '-' || token[0] == '*');
}

void evaluate(char op) {
    Ctxt *op1, *op2;

	switch(op) {
		case '+':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) += (*op2);
			theStack.push(*op1);
			break;
		case '-':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) -= (*op2);
			theStack.push(*op1);
			break;
		case '*':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) *= (*op2);
			theStack.push(*op1);
			break;
	}
}

void setupHELib() {
    long p=101;
    long r=1;
    long L=8;
    long c=2;
    long k=80;
    long s=0;
    long d=0;
    long w=64;
    long m = FindM(k, L, c, p, d, s, 0);
    
    context = new FHEcontext(m,p,r);
    buildModChain(*context, L, c);
    G = context->alMod.getFactorsOverZZ()[0];

    secretKey = new FHESecKey(*context);
    publicKey = secretKey;

   secretKey->GenSecKey(w); 
   addSome1DMatrices(*secretKey); // compute key-switching matrices that we need
}
