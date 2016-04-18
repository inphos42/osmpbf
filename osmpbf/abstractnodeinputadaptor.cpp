#include <osmpbf/abstractnodeinputadaptor.h>

namespace osmpbf {

void AbstractNodeInputAdaptor::print(std::ostream& out) const
{
	out << "{\n";
	printCommon(out);
	out << "\tlat:" << this->latd() << '\n';
	out << "\tlon:" << this->lond() << '\n';
	out << "}";
}


}//end namespace osmpbf