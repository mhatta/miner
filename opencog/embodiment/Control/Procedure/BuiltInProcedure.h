/*
 * opencog/embodiment/Control/Procedure/BuiltInProcedure.h
 *
 * Copyright (C) 2007-2008 Welter Luigi
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef BUILTINPROCEDURE_H_
#define BUILTINPROCEDURE_H_
/**
 * Base class for BuiltIn Procedures
 * @author Welter Luigi
 * 
 */
#include "GeneralProcedure.h" 
#include "comboreduct/combo/vertex.h"
#include <list> 

namespace Procedure {
    
class BuiltInProcedure : public GeneralProcedure {

protected:

    unsigned int minArity;
    unsigned int optionalArity;

public: 
    virtual ~BuiltInProcedure(){}
      
    virtual combo::vertex execute(const std::vector<combo::vertex>& arguments) const=0;
    
    ProcedureType getType() const {
        return BUILT_IN;
    }

    /**
     * Indicates if this procedure is an Pet action schemata. 
     * If so, its execute method always return the ActionPlanId for the action sent to Proxy
     */ 
    virtual bool isPetAction() const {
        return false;
    }
    
    /**
     * Return the mandatory arity for the builtin action
     */ 
    unsigned int getArity() const { return minArity; }

    /**
     * Return the optional arity for the buitlin action
     */
    unsigned int getOptionalArity() { return optionalArity; }
    
    /**
     * Return the max arity (min + optional arities) for the builtin action
     */
    unsigned int getMaxArity() const { return (minArity + optionalArity); }
};

} 

#endif /*BUILTINPROCEDURE_H_*/
