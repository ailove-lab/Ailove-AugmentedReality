#include "PatternManager.h"

//-------------------------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------------------------

PatternManager::PatternManager(void)
{
}

//-------------------------------------------------------------------------------------

PatternManager::~PatternManager(void)
{
}

//-------------------------------------------------------------------------------------

void PatternManager::registerPattern(const std::string& name, int id)

{
    associatedNodes.push_back(PatternToNode(name, id));
}

//-------------------------------------------------------------------------------------

void PatternManager::updateIds(const std::vector<int>& ids)
    
{
    for(std::vector<int>::const_iterator it = ids.begin(); ids.end() != it; ++it) {

        AssociatedNodes::iterator jt = associatedNodes.begin();
        for(; associatedNodes.end() != jt; ++jt) {
            if (jt->id == *it) {
                break;
            }
        }
        
        // if pattern does not have an id then it gets pattern's id founded on the image
        if (jt == associatedNodes.end()) {
            for(AssociatedNodes::iterator jt = associatedNodes.begin(); associatedNodes.end() != jt; ++jt) {
                if (jt->id == -1) {
                    jt->id = *it;
                    break;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------

const PatternManager::PatternToNode* PatternManager::get(int id) const

{
    // number of patterns defined very small thus it is working fast

    for(AssociatedNodes::const_iterator it = associatedNodes.begin(); associatedNodes.end() != it; ++it) {
        if (it->id == id) {
            return &(*it);
        }
    }

    // returns first automatically associated pattern;
    for(AssociatedNodes::const_iterator it = associatedNodes.begin(); associatedNodes.end() != it; ++it) {
        if (it->id == -1) {
            return &(*it);
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------