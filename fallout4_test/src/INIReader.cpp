//////////////////////////////////////////
/*
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#include "INIReader.h"

#if FALLOUT4_MINI_PROJECT

#include "common.h"

mINIReader::mINIReader(const std::string& fname): m_INIFile(mINI::INIFile(fname)), bChanged(false)
{
	Reload();
}

INI_BOOL mINIReader::GetBoolean(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_BOOL ValueDef)
{
	INI_STR valstr = XUtil::Str::LowerCase(Get(section, field, ""));

	if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
		return true;
	else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
		return false;
	else
		return ValueDef;
}

INI_STR mINIReader::Get(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_STR ValueDef)
{
	if (m_INI.has(section))
	{
		auto sec = m_INI.get(section);
		if (sec.has(field))
			return sec.get(field);
			
		return ValueDef;
	}

	return ValueDef;
}

INI_FLOAT mINIReader::GetFloat(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_FLOAT ValueDef)
{
	char* end;
	sprintf(szBuf, "%f", ValueDef);
	INI_STR val = Get(section, field, szBuf);
	return (INI_FLOAT)strtof(val.c_str(), &end);
}

INI_REAL mINIReader::GetReal(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_REAL ValueDef)
{
	char* end;
	sprintf(szBuf, "%f", ValueDef);
	INI_STR val = Get(section, field, szBuf);
	return (INI_REAL)strtod(val.c_str(), &end);
}

INI_INT mINIReader::GetInteger(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_INT ValueDef)
{
	char* end;
	INI_STR val = Get(section, field, _i64toa(ValueDef, szBuf, 10));
	return (INI_INT)strtol(val.c_str(), &end, 10);
}

void mINIReader::Set(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_STR Value)
{
	bChanged = true;
	m_INI[section][field] = Value;
}

void mINIReader::SetBoolean(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_BOOL Value)
{
	Set(section, field, Value ? "true" : "false");
}

void mINIReader::SetFloat(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_FLOAT Value)
{
	sprintf(szBuf, "%f", Value);
	Set(section, field, szBuf);
}

void mINIReader::SetReal(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_REAL Value)
{
	sprintf(szBuf, "%f", Value);
	Set(section, field, szBuf);
}

void mINIReader::SetInteger(INI_SECTION_NAME& section, INI_FIELD_NAME& field, INI_INT Value)
{
	sprintf(szBuf, "%d", Value);
	Set(section, field, szBuf);
}

void mINIReader::Reload(void)
{
	m_INIFile.read(m_INI);
}

void mINIReader::Save(void)
{
	if (bChanged)
	{
		bChanged = false;
		m_INIFile.write(m_INI, true);
	}
}

mINIReader::~mINIReader(void)
{
	Save();
}

#endif