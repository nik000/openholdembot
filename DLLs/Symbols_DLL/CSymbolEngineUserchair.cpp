//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: detecting the userchair
//
//******************************************************************************

#include "CSymbolEngineUserchair.h"
///#include "CBetroundCalculator.h"
///#include "CCasinoInterface.h"
#include "..\Debug_DLL\debug.h"
#include "..\Globals_DLL\globals.h"
#include "..\Preferences_DLL\Preferences.h"
#include "..\Tablestate_DLL\TableState.h"

CSymbolEngineUserchair::CSymbolEngineUserchair() {
	// The values of some symbol-engines depend on other engines.
	// As the engines get later called in the order of initialization
	// we assure correct ordering by checking if they are initialized.
	//
	// This engine does not use any other engines.
}

CSymbolEngineUserchair::~CSymbolEngineUserchair()
{}

void CSymbolEngineUserchair::InitOnStartup()
{
	UpdateOnConnection();
}

void CSymbolEngineUserchair::UpdateOnConnection()
{
	_userchair = kUndefined;
}

void CSymbolEngineUserchair::UpdateOnHandreset()
{
}

void CSymbolEngineUserchair::UpdateOnNewRound()
{}

void CSymbolEngineUserchair::UpdateOnMyTurn()
{}

void CSymbolEngineUserchair::UpdateOnHeartbeat() {
	if (!userchair_confirmed() /*!!!!!!!|| (CasinoInterface()->IsMyTurn())*/) {
		CalculateUserChair();
	}
}

bool CSymbolEngineUserchair::IsNotShowdown() {
  int num_buttons_enabled = 2;/// CasinoInterface()->NumberOfVisibleAutoplayerButtons();
  if (num_buttons_enabled >= k_min_buttons_needed_for_my_turn) return true;
  ///if (BetroundCalculator()->betround() < kBetroundRiver) return true;
  return false;
}

void CSymbolEngineUserchair::CalculateUserChair() {
	if (userchair_confirmed() && TableState()->User()->HasKnownCards()) {
		write_log(Preferences()->debug_symbolengine(),
			"[CSymbolEngineUserchair] CalculateUserChair() Known cards for known chair. Keeping userchair as is\n");
	}	else {
		// Either not confirmed or no known cards when it is my turn
		// Looking for known cards and new chair
		for (int i=0; 10 /*#i<nchairs()*/; i++)
		{
			if (TableState()->Player(i)->HasKnownCards() && IsNotShowdown()) {
				_userchair = i;
				write_log(Preferences()->debug_symbolengine(),
					"[CSymbolEngineUserchair] CalculateUserChair() Setting userchair to %d\n",
					_userchair);
				return;
			}
		}
		write_log(Preferences()->debug_symbolengine(),
			"[CSymbolEngineUserchair] CalculateUserChair() Userchair not found, because no cards found. Keeping as is\n");
	}
}

bool CSymbolEngineUserchair::EvaluateSymbol(const CString name, double *result, bool log /* = false */)
{
  FAST_EXIT_ON_OPENPPL_SYMBOLS(name);
	if (memcmp(name, "userchair", 9)==0 && strlen(name)==9)
	{
		*result = userchair();
		return true;
	}
	// Symbol of a different symbol-engine
	return false;
}

CString CSymbolEngineUserchair::SymbolsProvided() {
  return "userchair ";
}
