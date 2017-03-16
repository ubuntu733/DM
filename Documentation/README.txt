README
------

Changes: hub pgm file - datetime added ParseInt
DateTime4 is all new
Helios changed again

[ ] put the predefined binding filters in their own rightful location 


Grounding Model design 

CGroundingManagerAgent class
    * Actions: CGroundingAction (the set of grounding actions/strategies)
                - stored in a hash indexed by name
    * functions for accesing actions by name
    * GroundingModel: CGroundingModel 
    
CGroundingAction
    * Run(InstanceData)
    * GetGenericName()
    * GetInstanceName()
    
    
CGroundingState
    * STRING2STRING hash holding grounding state description
    
CGroundingBeliefState
    * Probability distribution over Grounding States    
    
CGroundingModel
    SaveToFile, LoadFromFile - persistance implementation
    Run() - estimates actions, takes the decision, learns, updates, etc
    
    CGroundingBeliefState ComputeBeliefState() - computes a probability distribution
        over belief states
    CActionDistribution ComputeActionDistribution(CBeliefState) - computes a probability distribution
        over the possible actions, given a belief state
    RunAction(actionIndex) runs a particular action