/*
 * Implicator.cc
 *
 * Copyright (C) 2009, 2014 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
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

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atomspace/SimpleTruthValue.h>
#include <opencog/atoms/bind/BindLink.h>

#include "BindLink.h"
#include "DefaultImplicator.h"
#include "PatternMatch.h"

using namespace opencog;

/**
 * This callback takes the reported grounding, runs it through the
 * instantiator, to create the implicand, and then records the result in
 * the public member `result_list`.  It then returns false, to search
 * for more groundings.  (The engine will halt its search for a
 * grounding once an acceptable one has been found; so, to continue
 * hunting for more, we return `false` here. We want to find all
 * possible groundings.)
 */
bool Implicator::grounding(const std::map<Handle, Handle> &var_soln,
                           const std::map<Handle, Handle> &term_soln)
{
	// PatternMatchEngine::print_solution(term_soln,var_soln);
	Handle h = inst.instantiate(implicand, var_soln);
	if (Handle::UNDEFINED != h)
	{
		result_list.push_back(h);
	}
	return false;
}

/**
 * The crisp implicator needs to tweak the truth value of the
 * resulting implicand. In most cases, this is not (strictly) needed,
 * for example, if the implicand has ungrounded variables, then
 * a truth value can be assigned to it, and the implicand will obtain
 * that truth value upon grounding.
 *
 * HOWEVER, if the implicand is fully grounded, then it will be given
 * a truth value of (false, uncertain) to start out with, and, if a
 * solution is found, then the goal here is to change its truth value
 * to (true, certain).  That is the whole point of this function:
 * to tweak (affirm) the truth value of existing clauses!
 */
bool CrispImplicator::grounding(const std::map<Handle, Handle> &var_soln,
                                const std::map<Handle, Handle> &term_soln)
{
	// PatternMatchEngine::print_solution(term_soln,var_soln);
	Handle h = inst.instantiate(implicand, var_soln);

	if (h != Handle::UNDEFINED)
	{
		result_list.push_back(h);

		// Set truth value to true+confident
		TruthValuePtr stv(SimpleTruthValue::createTV(1, SimpleTruthValue::confidenceToCount(1)));
		h->setTruthValue(stv);
	}
	return false;
}

/**
 * The single implicator behaves like the default implicator, except that
 * it terminates after the first solution is found.
 */
bool SingleImplicator::grounding(const std::map<Handle, Handle> &var_soln,
                                 const std::map<Handle, Handle> &term_soln)
{
	Handle h = inst.instantiate(implicand, var_soln);

	if (h != Handle::UNDEFINED)
	{
		result_list.push_back(h);
	}
	return true;
}

namespace opencog
{

/* Simplified utility */
static Handle do_imply(AtomSpace* as,
                       const Handle& hbindlink,
                       Implicator& impl,
                       bool do_conn_check=true)
{
	BindLinkPtr bl(BindLinkCast(hbindlink));
	if (NULL == bl)
		bl = createBindLink(*LinkCast(hbindlink));

	impl.implicand = bl->get_implicand();
	impl.set_type_restrictions(bl->get_variables().typemap);

	bl->imply(&impl, do_conn_check);

	// The result_list contains a list of the grounded expressions.
	// (The order of the list has no significance, so it's really a set.)
	// Put the set into a SetLink, and return that.
	Handle gl = as->addLink(SET_LINK, impl.result_list);
	return gl;
}

/**
 * Evaluate an ImplicationLink embedded in a BindLink
 *
 * Use the default implicator to find pattern-matches. Associated truth
 * values are completely ignored during pattern matching; if a set of
 * atoms that could be a ground are found in the atomspace, then they
 * will be reported.
 *
 * See the do_bindlink function documentation for details.
 */
Handle bindlink(AtomSpace* as, const Handle& hbindlink)
{
	// Now perform the search.
	DefaultImplicator impl(as);
	return do_imply(as, hbindlink, impl);
}

/**
 * Evaluate an ImplicationLink embedded in a BindLink
 *
 * Returns the first match only. Otherwise, the behavior is identical to
 * PatternMatch::bindlink above.
 *
 * See the do_bindlink function documentation for details.
 */
Handle single_bindlink (AtomSpace* as, const Handle& hbindlink)
{
	// Now perform the search.
	SingleImplicator impl(as);
	return do_imply(as, hbindlink, impl);
}

/**
 * Evaluate an ImplicationLink embedded in a BindLink
 *
 * Use the crisp-logic callback to evaluate boolean implication
 * statements; i.e. statements that have truth values assigned
 * their clauses, and statements that start with NotLink's.
 * These are evaluated using "crisp" logic: if a matched clause
 * is true, its accepted, if its false, its rejected. If the
 * clause begins with a NotLink, true and false are reversed.
 *
 * The NotLink is also interpreted as an "absence of a clause";
 * if the atomspace does NOT contain a NotLink clause, then the
 * match is considered postive, and the clause is accepted (and
 * it has a null or "invalid" grounding).
 *
 * See the do_bindlink function documentation for details.
 */
Handle crisp_logic_bindlink(AtomSpace* as, const Handle& hbindlink)
{
	// Now perform the search.
	CrispImplicator impl(as);
	return do_imply(as, hbindlink, impl);
}

/**
 * PLN specific PatternMatchCallback implementation
 */
Handle pln_bindlink(AtomSpace* as, const Handle& hbindlink)
{
	// Now perform the search.
	PLNImplicator impl(as);
	return do_imply(as, hbindlink, impl, false);
}

}

/* ===================== END OF FILE ===================== */
