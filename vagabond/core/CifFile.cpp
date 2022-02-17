#include <iostream>
#include <gemmi/numb.hpp>
#include "commit.h"

#include "CifFile.h"
#include "RefList.h"
#include "Knotter.h"
#include "GeometryTable.h"
#include "Atom.h"
#include "AtomGroup.h"
#include "../utils/glm_import.h"

using namespace gemmi::cif;

std::string CifFile::macroHeaders[] = 
{
	"_atom_site.group_PDB",         /* e.g. ATOM or HETATM */
	"_atom_site.id",                /* atom number */
	"_atom_site.type_symbol",       /* element symbol */
	"_atom_site.label_alt_id",      /* alternative conformer */
	"_atom_site.label_atom_id",     /* atom name */
	"_atom_site.label_comp_id",     /* chemical component i.e. residue */
	"_atom_site.label_asym_id",     /* seems to be chain ID */
	"_atom_site.Cartn_x",           /* cartesian coordinate x*/
	"_atom_site.Cartn_y",           /* cartesian coordinate y*/
	"_atom_site.Cartn_z",           /* cartesian coordinate z*/
	"_atom_site.occupancy",
	"_atom_site.B_iso_or_equiv"
};

CifFile::CifFile(std::string filename)
{
	changeFilename(filename);

	_table = new GeometryTable();
	_compAtoms = new AtomGroup();
	_macroAtoms = new AtomGroup();
	_accessedCompAtoms = false;
	_accessedTable = false;
	_knot = true;
}

void CifFile::changeFilename(std::string filename)
{
	_filename = filename;
	if (filename == "")
	{
		return;
	}
}

CifFile::~CifFile()
{
	if (!_accessedCompAtoms)
	{
		delete _compAtoms;
	}

	if (!_accessedMacroAtoms)
	{
		delete _macroAtoms;
	}

	else if (!_accessedTable)
	{
		delete _table;
	}
}

std::string toFilename(std::string filename)
{
	std::string tmp = filename;
#ifndef __EMSCRIPTEN__
	tmp = std::string(DATA_DIRECTORY) + "/" + filename;
#endif

	return tmp;
}

void CifFile::parseFileContents(std::string file)
{
	std::string tmp = toFilename(file);

	Document doc;
	try
	{
		doc = read_file(tmp);
	}
	catch (std::runtime_error err)
	{
		std::cout << "Could not load file " << file << std::endl;
		std::cout << err.what() << std::endl;
		return;
	}

	for (size_t j = 0; j < doc.blocks.size(); j++)
	{
		Block &contents = doc.blocks[j];
		for (size_t i = 0; i < contents.items.size(); i++)
		{
			if (contents.items[i].type == ItemType::Loop)
			{
				Loop &loop = contents.items[i].loop;

				processLoop(loop);
			}
			else if (contents.items[i].type == ItemType::Pair)
			{
				Pair &pair = contents.items[i].pair;

				processPair(pair);
			}
		}
	}
}

void CifFile::parse()
{
	parseFileContents(_filename);
	
	if (_knot && _macroAtoms->size() > 0)
	{
		parseFileContents("assets/geometry/standard_geometry.cif");
	}
	
	if (_knot)
	{
		AtomGroup *ptr = (_macroAtoms->size() == 0 ? _compAtoms : _macroAtoms);
		Knotter knotter(ptr, _table);
		knotter.knot();
	}
}

void CifFile::processPair(Pair &pair)
{
	_values[pair[0]] = pair[1];
}

void CifFile::processLoop(Loop &loop)
{
	if (processLoopAsCompAtoms(loop))
	{
		return;
	}

	if (processLoopAsLengths(loop))
	{
		return;
	}

	if (processLoopAsAngles(loop))
	{
		return;
	}
	
	if (processLoopAsLengthLinks(loop))
	{
		return;
	}
	
	if (processLoopAsAngleLinks(loop))
	{
		return;
	}

	if (processLoopAsTorsions(loop))
	{
		return;
	}

	if (processLoopAsChirals(loop))
	{
		return;
	}

	if (processLoopAsMacroAtoms(loop))
	{
		return;
	}
}

bool CifFile::identifyHeader(Loop &loop, std::string headers[])
{
	int n = sizeof(*headers) / sizeof(std::string);

	for (size_t j = 0; j < loop.tags.size(); j++)
	{
		bool found = false;

		for (size_t i = 0; i < n; i++)
		{
			if (loop.tags[j] == headers[i])
			{
				found = true;
			}
		}
		
		if (!found)
		{
			return false;
		}
	}
	
	return true;

}

bool CifFile::identifyHeader(Document &doc, std::string headers[])
{
	for (size_t j = 0; j < doc.blocks.size(); j++)
	{
		Block &contents = doc.blocks[j];
		for (size_t i = 0; i < contents.items.size(); i++)
		{
			if (contents.items[i].type == ItemType::Loop)
			{
				Loop &loop = contents.items[i].loop;
				bool found = identifyHeader(loop, headers);
				
				if (found)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CifFile::getHeaders(Loop &loop, std::string *headers, int *indices,
                              int n)
{
	for (size_t i = 0; i < n; i++)
	{
		indices[i] = -1;
	}

	for (size_t j = 0; j < loop.tags.size(); j++)
	{
		for (size_t i = 0; i < n; i++)
		{
			if (loop.tags[j] == headers[i])
			{
				indices[i] = j;
			}
		}
	}

	bool found = true;
	for (size_t i = 0; i < n; i++)
	{
		if (indices[i] == -1)
		{
			found = false;
		}
	}
	
	return found;
}

bool CifFile::processLoopAsChirals(Loop &loop)
{
	int idxs[6];
	int &code_idx = idxs[0];
	int &ctr_idx = idxs[1];
	int &id1_idx = idxs[2];
	int &id2_idx = idxs[3];
	int &id3_idx = idxs[4];
	int &sign_idx = idxs[5];
	
	std::string headers[] = 
	{
		"_chem_comp_chir.comp_id", "_chem_comp_chir.atom_id_centre",
		"_chem_comp_chir.atom_id_1", "_chem_comp_chir.atom_id_2",
		"_chem_comp_chir.atom_id_3", "_chem_comp_chir.volume_sign"
	};
	
	if (!getHeaders(loop, headers, idxs, 6))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		if (_code.length() && loop.values[i + code_idx] != _code)
		{
			continue;
		}
		
		std::string code = loop.values[i + code_idx];
		std::string centre = loop.values[i + ctr_idx];
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		std::string r = loop.values[i + id3_idx];
		std::string sign_str = (loop.values[i + sign_idx]);
		to_lower(sign_str);

		int sign = 0;
		if (sign_str.rfind("pos", 0) != std::string::npos)
		{
			sign = 1;
		}
		else if (sign_str.rfind("neg", 0) != std::string::npos)
		{
			sign = -1;
		}

		_table->addGeometryChiral(code, centre, p, q, r, sign);
	}
	
	return true;
}

bool CifFile::processLoopAsCompAtoms(Loop &loop)
{
	int idxs[6];
	int &code_idx = idxs[0];
	int &x_idx = idxs[1];
	int &y_idx = idxs[2];
	int &z_idx = idxs[3];
	int &atom_name_idx = idxs[4];
	int &ele_idx = idxs[5];
	
	std::string headers[] = 
	{
		"_chem_comp_atom.comp_id", "_chem_comp_atom.x",
		"_chem_comp_atom.y", "_chem_comp_atom.z",
		"_chem_comp_atom.atom_id", "_chem_comp_atom.type_symbol"
	};
	
	if (!getHeaders(loop, headers, idxs, 6))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		if (_code.length() && loop.values[i + code_idx] != _code)
		{
			continue;
		}
		
		std::string code = loop.values[i + code_idx];
		glm::vec3 pos = glm::vec3(0.);
		pos.x = as_number(loop.values[i + x_idx]);
		pos.y = as_number(loop.values[i + y_idx]);
		pos.z = as_number(loop.values[i + z_idx]);

		std::string ele = loop.values[i + ele_idx];
		std::string name = loop.values[i + atom_name_idx];

		Atom *a = new Atom();
		a->setElementSymbol(ele);
		a->setAtomName(name);
		a->setInitialPosition(pos, 30);
		a->setCode(code);
		
		_compAtoms->add(a);
	}
	
	return true;
}

bool CifFile::processLoopAsAngles(Loop &loop)
{
	int idxs[6];
	int &code_idx = idxs[0];
	int &id1_idx = idxs[1];
	int &id2_idx = idxs[2];
	int &id3_idx = idxs[3];
	int &angle_idx = idxs[4];
	int &dev_idx = idxs[5];
	
	std::string headers[] = 
	{
		"_chem_comp_angle.comp_id", "_chem_comp_angle.atom_id_1", 
		"_chem_comp_angle.atom_id_2", "_chem_comp_angle.atom_id_3", 
		"_chem_comp_angle.value_angle", "_chem_comp_angle.value_angle_esd"
	};
	
	if (!getHeaders(loop, headers, idxs, 6))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		if (_code.length() && loop.values[i + code_idx] != _code)
		{
			continue;
		}
		
		std::string code = loop.values[i + code_idx];
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		std::string r = loop.values[i + id3_idx];
		float angle = as_number(loop.values[i + angle_idx]);
		float dev = as_number(loop.values[i + dev_idx]);

		_table->addGeometryAngle(code, p, q, r, angle, dev);
	}
	
	return true;
}

bool CifFile::processLoopAsAngleLinks(Loop &loop)
{
	int idxs[6];
	int &id1_idx = idxs[0];
	int &id2_idx = idxs[1];
	int &id3_idx = idxs[2];
	int &angle_idx = idxs[3];
	int &dev_idx = idxs[4];
	int &link_idx = idxs[5];
	
	std::string headers[] = 
	{
		"_chem_link_angle.atom_id_1", "_chem_link_angle.atom_id_2", 
		"_chem_link_angle.atom_id_3", "_chem_link_angle.value_angle", 
		"_chem_link_angle.value_angle_esd", "_chem_link_angle.link_id"
	};
	
	if (!getHeaders(loop, headers, idxs, 6))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		std::string r = loop.values[i + id3_idx];
		float angle = as_number(loop.values[i + angle_idx]);
		float dev = as_number(loop.values[i + dev_idx]);
		std::string link_code = loop.values[i + link_idx];
		
		for (size_t j = 0; j < link_code.size(); j++)
		{
			if (link_code[j] == '\"')
			{
				link_code.erase(link_code.begin() + j);
				j--;
			}
		}

		_table->addGeometryAngle(link_code, p, q, r, angle, dev, true);
	}
	
	return true;
}

bool CifFile::processLoopAsLengthLinks(Loop &loop)
{
	int idxs[5];
	int &id1_idx = idxs[0];
	int &id2_idx = idxs[1];
	int &dist_idx = idxs[2];
	int &dev_idx = idxs[3];
	int &link_idx = idxs[4];
	
	std::string headers[] = 
	{
		"_chem_link_bond.atom_id_1", "_chem_link_bond.atom_id_2", 
		"_chem_link_bond.value_dist", "_chem_link_bond.value_dist_esd",
		"_chem_link_bond.link_id"
	};
	
	if (!getHeaders(loop, headers, idxs, 5))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		float length = as_number(loop.values[i + dist_idx]);
		float dev = as_number(loop.values[i + dev_idx]);
		std::string link_code = loop.values[i + link_idx];
		
		for (size_t j = 0; j < link_code.size(); j++)
		{
			if (link_code[j] == '\"')
			{
				link_code.erase(link_code.begin() + j);
				j--;
			}
		}

		_table->addGeometryLength(link_code, p, q, length, dev, true);
	}
	
	return true;
}

bool CifFile::processLoopAsLengths(Loop &loop)
{
	int idxs[5];
	int &code_idx = idxs[0];
	int &id1_idx = idxs[1];
	int &id2_idx = idxs[2];
	int &dist_idx = idxs[3];
	int &dev_idx = idxs[4];
	
	std::string headers[] = 
	{
		"_chem_comp_bond.comp_id", "_chem_comp_bond.atom_id_1", 
		"_chem_comp_bond.atom_id_2", "_chem_comp_bond.value_dist", 
		"_chem_comp_bond.value_dist_esd"
	};
	
	if (!getHeaders(loop, headers, idxs, 5))
	{
		return false;
	}
	
	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		if (_code.length() && loop.values[i + code_idx] != _code)
		{
			continue;
		}
		
		std::string code = loop.values[i + code_idx];
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		float length = as_number(loop.values[i + dist_idx]);
		float dev = as_number(loop.values[i + dev_idx]);

		_table->addGeometryLength(code, p, q, length, dev);
	}
	
	return true;
}

bool CifFile::processLoopAsTorsions(Loop &loop)
{
	int idxs[8];
	int &code_idx = idxs[0];
	int &id1_idx = idxs[1];
	int &id2_idx = idxs[2];
	int &id3_idx = idxs[3];
	int &id4_idx = idxs[4];
	int &angle_idx = idxs[5];
	int &dev_idx = idxs[6];
	int &period_idx = idxs[7];
	
	std::string headers[] = 
	{
		"_chem_comp_tor.comp_id", "_chem_comp_tor.atom_id_1", 
		"_chem_comp_tor.atom_id_2", "_chem_comp_tor.atom_id_3", 
		"_chem_comp_tor.atom_id_4", "_chem_comp_tor.value_angle",
		"_chem_comp_tor.value_angle_esd", "_chem_comp_tor.period"
	};
	
	if (!getHeaders(loop, headers, idxs, 8))
	{
		return false;
	}

	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		if (_code.length() && loop.values[i + code_idx] != _code)
		{
			continue;
		}
		
		std::string code = loop.values[i + code_idx];
		std::string p = loop.values[i + id1_idx];
		std::string q = loop.values[i + id2_idx];
		std::string r = loop.values[i + id3_idx];
		std::string s = loop.values[i + id4_idx];
		float angle = as_number(loop.values[i + angle_idx]);
		float dev = as_number(loop.values[i + dev_idx]);
		int period = as_number(loop.values[i + period_idx]);

		_table->addGeometryTorsion(code, p, q, r, s, angle, dev, period);
	}
	
	return true;
}

bool CifFile::processLoopAsMacroAtoms(Loop &loop)
{
	int idxs[12];
	int &card_idx = idxs[0];
	int &num_idx = idxs[1];
	int &ele_idx = idxs[2];
	int &alt_idx = idxs[3];
	int &atom_name_idx = idxs[4];
	int &res_idx = idxs[5];
	int &chain_idx = idxs[6];
	int &x_idx = idxs[7];
	int &y_idx = idxs[8];
	int &z_idx = idxs[9];
	int &occ_idx = idxs[10];
	int &b_idx = idxs[11];
	
	if (!getHeaders(loop, macroHeaders, idxs, 12))
	{
		return false;
	}

	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		std::string alt = loop.values[i + alt_idx];
		
		if (alt != "." && alt != "A")
		{
			continue;
		}

		glm::vec3 pos = glm::vec3(0.);
		pos.x = as_number(loop.values[i + x_idx]);
		pos.y = as_number(loop.values[i + y_idx]);
		pos.z = as_number(loop.values[i + z_idx]);

		int num = as_number(loop.values[i + num_idx]);
		std::string ele = loop.values[i + ele_idx];
		std::string name = loop.values[i + atom_name_idx];
		std::string code = loop.values[i + res_idx];
		std::string chain = loop.values[i + chain_idx];
		float occ = as_number(loop.values[i + occ_idx]);
		float b = as_number(loop.values[i + b_idx]);
		
		std::string card = loop.values[i + card_idx];
		bool hetatm = (card == "HETATM");

		Atom *a = new Atom();
		a->setElementSymbol(ele);
		a->setAtomName(name);
		a->setAtomNum(num);
		a->setHetatm(hetatm);
		a->setInitialPosition(pos, b);
		a->setOccupancy(occ);
		a->setCode(code);
		
		_macroAtoms->add(a);
	}

	return true;
}

bool CifFile::processLoopAsReflections(Loop &loop)
{
	int idxs[7];
	int &h_idx = idxs[0];
	int &k_idx = idxs[1];
	int &l_idx = idxs[2];
	int &free_idx = idxs[3];
	int &free_flag_idx = idxs[4];
	int &amp_idx = idxs[5];
	int &sigma_idx = idxs[6];
	
	std::string headers[] = 
	{
		"_refln.index_h",
		"_refln.index_k",
		"_refln.index_l",
		"_refln.status", /** o if working set, f if free set */
		"_refln.pdbx_r_free_flag",
		"_refln.F_meas_au",
		"_refln.F_meas_sigma_au",
	};
	
	if (!getHeaders(loop, headers, idxs, 7))
	{
		return false;
	}

	for (size_t i = 0; i < loop.values.size(); i += loop.tags.size())
	{
		int h = as_number(loop.values[i + h_idx]);
		int k = as_number(loop.values[i + k_idx]);
		int l = as_number(loop.values[i + l_idx]);
		
		bool free = (loop.values[i + free_idx] == "f");
		int  flag = as_number(loop.values[i + free_flag_idx]);

		float f    = as_number(loop.values[i + amp_idx]);
		float sigf = as_number(loop.values[i + sigma_idx]);

		Reflection refl{};
		refl.hkl.h = h;
		refl.hkl.k = k;
		refl.hkl.l = l;
		refl.free = free;
		refl.flag = flag;
		refl.f = f;
		refl.sigf = sigf;
		
		_reflections.push_back(refl);
	}

	return true;
}

const size_t CifFile::compAtomCount() const
{
	return _compAtoms->size();
}

const size_t CifFile::atomCount() const
{
	if (_macroAtoms && _macroAtoms->size() > 0)
	{
		return _macroAtoms->size();
	}
	if (_compAtoms && _compAtoms->size() > 0)
	{
		return _compAtoms->size();
	}
	
	return 0;
}

AtomGroup *CifFile::atoms()
{
	if (_macroAtoms && _macroAtoms->size() > 0)
	{
		_accessedMacroAtoms = true;
		return _macroAtoms;
	}
	else
	{
		_accessedCompAtoms = true;
		return _compAtoms;
	}
}

bool CifFile::hasUnitCell() const
{
	bool found = true;
	found &= _values.count("_cell.length_a");
	found &= _values.count("_cell.length_b");
	found &= _values.count("_cell.length_c");
	found &= _values.count("_cell.angle_alpha");
	found &= _values.count("_cell.angle_beta");
	found &= _values.count("_cell.angle_gamma");
	
	return found;
}

std::array<double, 6> CifFile::unitCell() const
{
	if (!hasUnitCell())
	{
		throw std::runtime_error("Requested non-existent unit cell");
	}

	double a = as_number(_values.at("_cell.length_a"));
	double b = as_number(_values.at("_cell.length_b"));
	double c = as_number(_values.at("_cell.length_c"));
	double alpha = as_number(_values.at("_cell.angle_alpha"));
	double beta = as_number(_values.at("_cell.angle_beta"));
	double gamma = as_number(_values.at("_cell.angle_gamma"));
	
	std::array<double, 6> cell = {a, b, c, alpha, beta, gamma};
	
	return cell;
}

int CifFile::spaceGroupNum() const
{
	if (_values.count("_symmetry.Int_Tables_number") == 0)
	{
		return -1;
	}

	int num = as_number(_values.at("_symmetry.Int_Tables_number"));
	return num;
}

RefList *CifFile::reflectionList() const
{
	RefList *list = new RefList(_reflections);
	list->setSpaceGroup(spaceGroupNum());
	
	if (hasUnitCell())
	{
		std::array<double, 6> cell = unitCell();
		list->setUnitCell(cell);
	}
	
	return list;
}

CifFile::Type CifFile::cursoryLook()
{
	std::string tmp = toFilename(_filename);
	Type type = Nothing;

	Document doc;
	try
	{
		doc = read_file(tmp);
	}
	catch (std::runtime_error err)
	{
		return type;
	}

	if (identifyHeader(doc, macroHeaders))
	{
		type = CifFile::Type(type | MacroAtoms);
	}
	
	return type;
}