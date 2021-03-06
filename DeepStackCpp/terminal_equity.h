#pragma once
#include <Eigen/Dense>
#include "assert.h"
#include "LeducEvaluator.h"
#include "game_settings.h"
#include "assert.h"

using namespace std;

//-- - Evaluates player equities at terminal nodes of the game's public tree.

class terminal_equity
{
public:
	terminal_equity();

	//-- - Zeroes entries in an equity matrix that correspond to invalid hands.
	//--
	//--A hand is invalid if it shares any cards with the board.
	//--
	//-- @param equity_matrix the matrix to modify
	//-- @param board a possibly empty vector of board cards
	void _handle_blocking_cards(ArrayXX& equity_matrix, const ArrayX& board);

	//-- - Constructs the matrix that turns player ranges into showdown equity.
	//	--
	//	--Gives the matrix `A` such that for player ranges `x` and `y`, `x'Ay` is the equity
	//	-- for the first player when no player folds.
	//	--
	//	-- @param board_cards a non - empty vector of board cards
	//	-- @param call_matrix a tensor where the computed matrix is stored
	void get_last_round_call_matrix(const ArrayX& board_cards, ArrayXX& call_matrix);

	//-- - Sets the board cards for the evaluator and creates its internal data structures.
	//-- @param board a possibly empty vector of board cards
	void set_board(const ArrayX& board);

	//-- - Sets the evaluator's call matrix, which gives the equity for terminal
	//--nodes where no player has folded.
	//--
	//--For nodes in the last betting round, creates the matrix `A` such that for player ranges
	//-- `x` and `y`, `x'Ay` is the equity for the first player when no player folds. For nodes
	//-- in the first betting round, gives the weighted average of all such possible matrices.
	//--
	//-- @param board a possibly empty vector of board cards
	void _set_call_matrix(const ArrayX& board);

	//-- - Sets the evaluator's fold matrix, which gives the equity for terminal
	//	--nodes where one player has folded.
	//	--
	//	--Creates the matrix `B` such that for player ranges `x` and `y`, `x'By` is the equity
	//	-- for the player who doesn't fold
	//	-- @param board a possibly empty vector of board cards
	void _set_fold_matrix(const ArrayX& board);

	template <typename Derived>
	void call_value(const ArrayBase<Derived> & ranges, ArrayBase<Derived> & result)
	{
		result = ranges.matrix() * _equity_matrix.matrix();
	}

	template <typename Derived>
	void fold_value(const ArrayBase<Derived> & ranges, ArrayBase<Derived> & result)
	{
		assert(_fold_matrix.size() > 0);
		result = (ranges.matrix() * _fold_matrix.matrix()).array();
	}

	//	-- - Computes the counterfactual values that both players achieve at a terminal node
	//-- where either player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a 2xK tensor containing ranges for each player(where K is the range size)
	//-- @param result a 2xK tensor in which to store the cfvs for each player
	//-- @param folding_player which player folded
	void tree_node_fold_value(const ArrayXX& ranges, ArrayXX& result, int folding_player);

	//-- - Returns the matrix which gives showdown equity for any ranges.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @return For nodes in the last betting round, the matrix `A` such that for player ranges
	//-- `x` and `y`, `x'Ay` is the equity for the first player when no player folds.For nodes
	//-- in the first betting round, the weighted average of all such possible matrices.
	ArrayXX get_call_matrix();

	//-- - Computes the counterfactual values that both players achieve at a terminal node
	//-- where no player has folded.
	//--
	//-- @{set_board
	//} must be called before this function.
	//--
	//-- @param ranges a 2xK tensor containing ranges for each player(where K is the range size)
	//-- @param result a 2xK tensor in which to store the cfvs for each player
	void tree_node_call_value(const ArrayXX& ranges, ArrayXX& result);


//private: ToDo:Remove after testing

	LeducEvaluator _evaluator;

	card_tools _cardTools;

	ArrayXX _equity_matrix;

	ArrayXX _fold_matrix;
};

//#include "terminal_equity.npp"