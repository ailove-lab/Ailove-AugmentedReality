#ifndef _PATTERNMANAGER_
#define _PATTERNMANAGER_

#include <vector>
#include <string>
#include <OgreMatrix4.h>
#include <set>


//-------------------------------------------------------------------------------------

/** Stores patterns */
class PatternManager {
public:
    
    /** Keeps patterns parameters. */
    struct PatternToNode {
        PatternToNode(const std::string& name, int id) 
            : nodeName(name), id(id){}
        int id;
        std::string nodeName;
    };

    typedef std::list<PatternToNode> AssociatedNodes;

    /** Default constructor. */
    PatternManager(void);
    ~PatternManager(void);

    /** Registers pattern attached to node with the given name.
      * if pattern id is -1 then founded patterns will be associated with it automatically.
      */
    void registerPattern(const std::string& name, int id = -1);

    /** Associates automatically defined patterns (where id is -1) with patterns founded on the image */
    void updateIds(const std::vector<int>& ids);

    const PatternToNode* get(int id) const;
private:
    AssociatedNodes associatedNodes;
};

//-------------------------------------------------------------------------------------

#endif

