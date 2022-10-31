#include "SpellBook.hpp"

SpellBook::SpellBook(){}

SpellBook::SpellBook(SpellBook const &copy){(void)copy;}

SpellBook const &SpellBook::operator=(SpellBook const &assign){(void)assign;return(*this);}

SpellBook::~SpellBook()
{
	std::map<std::string, ASpell*>::iterator it = _spellBook.begin();
	for (; it != _spellBook.end();++it)
		delete it->second;
	_spellBook.clear();
}

void SpellBook::learnSpell(ASpell* spell)
{
	if (spell)
		_spellBook.insert(std::pair<std::string, ASpell*>(spell->getName(), spell->clone()));
}

void SpellBook::forgetSpell(std::string const &spell)
{
	std::map<std::string, ASpell*>::iterator it = _spellBook.find(spell);
	if (it != _spellBook.end())
		delete it->second;
	_spellBook.erase(it);
}

ASpell* SpellBook::createSpell(std::string const &spell)
{
	std::map<std::string, ASpell*>::iterator it = _spellBook.find(spell);
	if (it != _spellBook.end())
		return (_spellBook[spell]);
	return (NULL);
}