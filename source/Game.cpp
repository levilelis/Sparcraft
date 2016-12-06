#include "Game.h"

using namespace SparCraft;

Game::Game(const GameState & initialState, const size_t & limit)
    : _numPlayers(0)
    , state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , rounds(0)
    , moveLimit(limit)
{

}

Game::Game(const GameState & initialState, PlayerPtr & p1, PlayerPtr & p2, const size_t & limit)
    : _numPlayers(0)
    , state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , rounds(0)
    , moveLimit(limit)
{
    // add the players
    _players[Players::Player_One] = p1;
    _players[Players::Player_Two] = p2;
}

// play the game until there is a winner
void Game::play()
{
    scriptMoves[Players::Player_One] = std::vector<Action>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<Action>(state.numUnits(Players::Player_Two));

    t.start();

    _players[Players::Player_One]->init(state, Players::Player_One);
    _players[Players::Player_Two]->init(state, Players::Player_Two);

    _players[Players::Player_One]->choosePolicy(state);
    _players[Players::Player_Two]->choosePolicy(state);

    // play until there is no winner
    while (!gameOver())
    {
        if (moveLimit && rounds >= moveLimit)
        {
            break;
        }

        playNextTurn();

        if(gameOver())
        {
        	_players[Players::Player_One]->updateStat(state);
        	_players[Players::Player_Two]->updateStat(state);
        }
    }

    _players[Players::Player_One]->finish();
    _players[Players::Player_Two]->finish();

    gameTimeMS = t.getElapsedTimeInMilliSec();
}

void Game::playNextTurn()
{
    Timer frameTimer;
    frameTimer.start();

    scriptMoves[0].clear();
    scriptMoves[1].clear();

    // the player that will move next
    const IDType playerToMove(getPlayerToMove());
    PlayerPtr & toMove = _players[playerToMove];
    PlayerPtr & enemy = _players[state.getEnemy(playerToMove)];

 //   toMove->updateStat(state);
 //   toMove->choosePolicy(state);

    // generate the moves possible from this state
    state.generateMoves(moves[toMove->ID()], toMove->ID());

    // the tuple of moves he wishes to make
    toMove->getMoves(state, moves[playerToMove], scriptMoves[playerToMove]);
        
    // if both players can move, generate the other player's moves
    if (state.bothCanMove())
    {
        state.generateMoves(moves[enemy->ID()], enemy->ID());
        enemy->getMoves(state, moves[enemy->ID()], scriptMoves[enemy->ID()]);

        state.makeMoves(scriptMoves[enemy->ID()]);
    }

    // make the moves
    state.makeMoves(scriptMoves[toMove->ID()]);

    state.finishedMoving();
    rounds++;
}

// play the game until there is a winner
const StateEvalScore Game::playLimitedIndividualScripts(const IDType & player, UnitScriptData & scriptData, int limit)
{
    // array which will hold all the script moves for players
    Array2D<std::vector<Action>, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

    scriptMoves[Players::Player_One] = std::vector<Action>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<Action>(state.numUnits(Players::Player_Two));

    t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if ((rounds > limit) || (moveLimit && rounds > moveLimit))
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear all script moves for both players
        for (IDType p(0); p<Constants::Num_Players; p++)
        {
            for (IDType s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // clear the moves we will actually be doing
        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the playr that will move next
        const IDType playerToMove(getPlayerToMove());
        const IDType enemyPlayer(state.getEnemy(playerToMove));

        // generate the moves possible from this state
        state.generateMoves(moves[playerToMove], playerToMove);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, moves[playerToMove], state, scriptMoves[playerToMove]);

        // if both players can move, generate the other player's moves
        if (state.bothCanMove())
        {
            state.generateMoves(moves[enemyPlayer], enemyPlayer);

            scriptData.calculateMoves(enemyPlayer, moves[enemyPlayer], state, scriptMoves[enemyPlayer]);

            state.makeMoves(scriptMoves[enemyPlayer]);
        }

        // make the moves
        state.makeMoves(scriptMoves[playerToMove]);
        state.finishedMoving();
        rounds++;
    }
    //StateEvalScore evalScore = state.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));

    gameTimeMS = t.getElapsedTimeInMilliSec();
    return state.eval(player, SparCraft::EvaluationMethods::Playout, PlayerModels::NOKDPS, PlayerModels::NOKDPS);
}

/*
const StateEvalScore Game::playoutGenome(const IDType & player, PortfolioOnlineGenome & genome, int limit)
{
    scriptMoves[Players::Player_One] = std::vector<Action>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<Action>(state.numUnits(Players::Player_Two));

	const IDType enemyPlayer(state.getEnemy(player));
	PlayerPtr enemy(AllPlayers::getPlayerPtr(enemyPlayer, PlayerModels::NOKDPS));

    t.start();
    int numberLevelsPlayed = 0;

    // play until there is no winner
    while (!gameOver())
    {

    //	std::cout << "FINISHED ROUND, number played: " << numberLevelsPlayed << std::endl;
        if (numberLevelsPlayed >= limit)
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear the moves we will actually be doing
        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the playr that will move next
        const IDType playerToMove(getPlayerToMove());
        const IDType enemyPlayer(state.getEnemy(playerToMove));

        // generate the moves possible from this state
        state.generateMoves(moves[playerToMove], playerToMove);

		//std::cout << "Player: " << (int) playerToMove << std::endl;

        //If the player is to move
        if(playerToMove == player)
        {
        	// calculate the moves the unit would do given its script preferences
        	genome.calculateMoves(moves[player], state, scriptMoves[player], 0);

    	//	std::cout << "Calculated moves from genome " << std::endl;

            // if both players can move, generate the other player's moves
            if (state.bothCanMove())
            {
          //  	std::cout << "both could move " << std::endl;
                //state.generateMoves(moves[enemyPlayer], enemyPlayer);
                //scriptData.calculateMoves(enemyPlayer, moves[enemyPlayer], state, scriptMoves[enemyPlayer]);

                state.generateMoves(moves[enemy->ID()], enemy->ID());
                enemy->getMoves(state, moves[enemy->ID()], scriptMoves[enemy->ID()]);
                state.makeMoves(scriptMoves[enemyPlayer]);
            }

     //       std::cout << "about to make moves " << std::endl;
            // make the moves
            state.makeMoves(scriptMoves[playerToMove]);
            state.finishedMoving();
            rounds++;
            numberLevelsPlayed++;
    //        std::cout << "made moves " << std::endl;
        }
        else //if the enemy is to move
        {
       // 	std::cout << "about to generate moves " << std::endl;
            state.generateMoves(moves[enemy->ID()], enemy->ID());
            enemy->getMoves(state, moves[enemy->ID()], scriptMoves[enemy->ID()]);
       //     std::cout << "generated and got moves " << std::endl;

            // if both players can move, generate the other player's moves
            if (state.bothCanMove())
            {
   //             std::cout << "both can move, player: " << (int) player << " number played: " << numberLevelsPlayed << std::endl;

            	genome.calculateMoves(moves[player], state, scriptMoves[player], 0);
                state.makeMoves(scriptMoves[player]);


     //           std::cout << "calculated moves and made them " << std::endl;
            }

      //      std::cout << "about to make moves" << std::endl;

            state.makeMoves(scriptMoves[enemy->ID()]);
            state.finishedMoving();
            rounds++;
            numberLevelsPlayed++;
     //       std::cout << "made moves " << std::endl;
        }
    }
    //StateEvalScore evalScore = state.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));
 //   std::cout << "About to start simulation" << std::endl;
    gameTimeMS = t.getElapsedTimeInMilliSec();
    //return state.evalSimLimited(player, PlayerModels::NOKDPS, PlayerModels::NOKDPS, 22);
    return state.eval(player, SparCraft::EvaluationMethods::LTD2, PlayerModels::NOKDPS, PlayerModels::NOKDPS);
    //return state.eval(player, SparCraft::EvaluationMethods::Playout, PlayerModels::NOKDPS, PlayerModels::NOKDPS);
}
*/

const StateEvalScore Game::playoutGenome(const IDType & player, PortfolioOnlineGenome & scriptData, int limit)
{
    // array which will hold all the script moves for players
    Array2D<std::vector<Action>, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

    scriptMoves[Players::Player_One] = std::vector<Action>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<Action>(state.numUnits(Players::Player_Two));

    t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if ((rounds > limit) || (moveLimit && rounds > moveLimit))
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear all script moves for both players
        for (IDType p(0); p<Constants::Num_Players; p++)
        {
            for (IDType s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // clear the moves we will actually be doing
        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the playr that will move next
        const IDType playerToMove(getPlayerToMove());
        const IDType enemyPlayer(state.getEnemy(playerToMove));

        // generate the moves possible from this state
        state.generateMoves(moves[playerToMove], playerToMove);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, moves[playerToMove], state, scriptMoves[playerToMove]);
        
	// if both players can move, generate the other player's moves
        if (state.bothCanMove())
        {
            state.generateMoves(moves[enemyPlayer], enemyPlayer);

            scriptData.calculateMoves(enemyPlayer, moves[enemyPlayer], state, scriptMoves[enemyPlayer]);

            state.makeMoves(scriptMoves[enemyPlayer]);
        }

        // make the moves
        state.makeMoves(scriptMoves[playerToMove]);
        state.finishedMoving();
        rounds++;
    }
    //StateEvalScore evalScore = state.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));

    gameTimeMS = t.getElapsedTimeInMilliSec();
    //return state.eval(player, SparCraft::EvaluationMethods::Playout, PlayerModels::NOKDPS, PlayerModels::NOKDPS);
    return state.evalSimLimited(player, PlayerModels::NOKDPS, PlayerModels::NOKDPS, 22);
}

// play the game until there is a winner
void Game::playIndividualScripts(UnitScriptData & scriptData)
{
    // array which will hold all the script moves for players
    Array2D<std::vector<Action>, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

    scriptMoves[Players::Player_One] = std::vector<Action>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<Action>(state.numUnits(Players::Player_Two));

    t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if (moveLimit && rounds > moveLimit)
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear all script moves for both players
        for (IDType p(0); p<Constants::Num_Players; p++)
        {
            for (IDType s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // clear the moves we will actually be doing
        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the playr that will move next
        const IDType playerToMove(getPlayerToMove());
        const IDType enemyPlayer(state.getEnemy(playerToMove));

        // generate the moves possible from this state
        state.generateMoves(moves[playerToMove], playerToMove);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, moves[playerToMove], state, scriptMoves[playerToMove]);

        // if both players can move, generate the other player's moves
        if (state.bothCanMove())
        {
            state.generateMoves(moves[enemyPlayer], enemyPlayer);

            scriptData.calculateMoves(enemyPlayer, moves[enemyPlayer], state, scriptMoves[enemyPlayer]);

            state.makeMoves(scriptMoves[enemyPlayer]);
        }

        // make the moves
        state.makeMoves(scriptMoves[playerToMove]);
        state.finishedMoving();
        rounds++;
    }

    gameTimeMS = t.getElapsedTimeInMilliSec();
}

PlayerPtr Game::getPlayer(const IDType & player)
{
    return _players[player];
}

int Game::getRounds()
{
    return rounds;
}

double Game::getTime()
{
    return gameTimeMS;
}

// returns whether or not the game is over
bool Game::gameOver() const
{
    return state.isTerminal(); 
}

const GameState & Game::getState() const
{
    return state;
}

GameState & Game::getState()
{
    return state;
}

// determine the player to move
const IDType Game::getPlayerToMove()
{
    const IDType whoCanMove(state.whoCanMove());

    return (whoCanMove == Players::Player_Both) ? Players::Player_One : whoCanMove;
}