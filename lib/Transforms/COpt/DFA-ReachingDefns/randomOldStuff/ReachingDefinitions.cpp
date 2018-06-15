#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include <iostream>
#include <algorithm>

#include "ReachingDefinitions.h"

using namespace llvm;
using namespace std;

namespace 
{

	class ReachingInfo : public Info //MyInfo
	{
	
		private:
		
		vector<int> defs;
		
		public:
		
		virtual void print()
		{
			for (int d: defs)
				cout<<d<<" ";
			cout<<"\n";
		}

		static bool compare(Info * inf1, Info * inf2)
		{
			return true;
		} 
    
    	static void join(Info * inf1, Info * inf2, Info * info)
    	{
    		
    	}
    
    	static void intersection(Info * inf1, Info * inf2, Info * info)
    	{
    	
    		rinf1 = (ReachingInfo *)inf1;
    		rinf2 = (ReachingInfo *)inf2;
    		info = (ReachingInfo *)info;
    		
    		sort(inf1->defs.begin(), inf1->defs.end());
    		sort(inf2->defs.begin(), inf2->defs.end());

    		set_intersection(inf1->defs.begin(),inf1->defs.end(),inf2->defs.begin(),inf2->defs.end(),back_inserter(info->defs));

    	}
		
	};}
int main()
{
	return 0;
}
	/*
	class ReachingDefinitionsAnalysis : public DataFlowAnalysis<ReachingInfo,true> DataFlowAnalysis//class MyForwardAnalysis
	{
		ReachingDefinitionsAnalysis(ReachingInfo &InitialStates, ReachingInfo &Bottom) 
		{
		
		}
	
	};
	struct RD : public FunctionPass
	{
    	static char ID;
    	RD() : FunctionPass(ID) {}

    	virtual bool runOnFunction(Function &F) 
    	{
    
    	}
    }
}

char ReachingDefintions::ID = 0;
static RegisterPass<ReachingDefinitions> X("rdef", "Reaching Definitions Pass");
*/
