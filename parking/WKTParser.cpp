#include "pch.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include "WKTParser.h"

Tokenizer::Tokenizer(const std::string &s)
	: str(s)
{}

Token Tokenizer::next()
{
	skipWhitespace();

	if (pos >= str.size())
		return { TokenType::End, "" };

	char c = str[pos];

	if (c == '(')
		{
		pos++;
		return { TokenType::LParen, "(" };
		}
	if (c == ')')
		{
		pos++;
		return { TokenType::RParen, ")" };
		}
	if (c == ',')
		{
		pos++;
		return { TokenType::Comma, "," };
		}

	if (std::isalpha(c))
		{
		size_t start = pos;
		while (pos < str.size() && std::isalpha(str[pos]))
			pos++;
		return { TokenType::Keyword, str.substr(start, pos - start) };
		}

	if (std::isdigit(c) || c == '-' || c == '+')
		{
		size_t start = pos;
		while (pos < str.size() &&
					 (std::isdigit(str[pos]) || str[pos] == '.' ||
						str[pos] == 'e' || str[pos] == 'E' ||
						str[pos] == '-' || str[pos] == '+'))
			pos++;
		return { TokenType::Number, str.substr(start, pos - start) };
		}

	throw std::runtime_error("Unexpected character in WKT");
}

Token Tokenizer::peek()
{
	size_t saved = pos;
	Token t = next();
	pos = saved;
	return t;
}

void Tokenizer::skipWhitespace()
{
	while (pos < str.size() && std::isspace(str[pos]))
		pos++;
}



WKTParser::WKTParser(const std::string &wkt)
	: tokenizer(wkt)
{}

void WKTParser::parse()
{
	parseGeometry();
}

void WKTParser::expect(TokenType type)
{
	Token t = tokenizer.next();
	if (t.type != type)
		throw std::runtime_error("Unexpected token");
}

WKTPoint WKTParser::parsePoint()
{
	Token tx = tokenizer.next();
	Token ty = tokenizer.next();

	if (tx.type != TokenType::Number || ty.type != TokenType::Number)
		throw std::runtime_error("Invalid coordinate");

	WKTPoint p{ std::stod(tx.text), std::stod(ty.text) };

	// Ignore optional Z
	if (tokenizer.peek().type == TokenType::Number)
		tokenizer.next();

	return p;
}

std::vector<WKTPoint> WKTParser::parsePointList()
{
	std::vector<WKTPoint> pts;
	expect(TokenType::LParen);

	do
		{
		pts.push_back(parsePoint());
		if (tokenizer.peek().type == TokenType::Comma)
			tokenizer.next();
		else
			break;
		}
	while (true);

	expect(TokenType::RParen);
	return pts;
}

void WKTParser::parseLineString()
{
	LineString ls;
	ls.WKTPoints = parsePointList();
	m_linestrings.push_back(ls);
}

void WKTParser::parsePolygon()
{
	WKTPolygon poly;
	expect(TokenType::LParen);

	do
		{
		poly.rings.push_back(parsePointList());
		if (tokenizer.peek().type == TokenType::Comma)
			tokenizer.next();
		else
			break;
		}
	while (true);

	expect(TokenType::RParen);
	m_polygons.push_back(poly);
}

void WKTParser::parseMultiLineString()
{
	expect(TokenType::LParen);

	do
		{
		LineString ls;
		ls.WKTPoints = parsePointList();
		m_linestrings.push_back(ls);

		if (tokenizer.peek().type == TokenType::Comma)
			tokenizer.next();
		else
			break;
		}
	while (true);

	expect(TokenType::RParen);
}

void WKTParser::parseMultiPolygon()
{
	expect(TokenType::LParen);

	do
		{
		WKTPolygon poly;
		expect(TokenType::LParen);

		do
			{
			poly.rings.push_back(parsePointList());
			if (tokenizer.peek().type == TokenType::Comma)
				tokenizer.next();
			else
				break;
			}
		while (true);

		expect(TokenType::RParen);
		m_polygons.push_back(poly);

		if (tokenizer.peek().type == TokenType::Comma)
			tokenizer.next();
		else
			break;
		}
	while (true);

	expect(TokenType::RParen);
}

void WKTParser::parseGeometryCollection()
{
	expect(TokenType::LParen);

	do
		{
		parseGeometry();
		if (tokenizer.peek().type == TokenType::Comma)
			tokenizer.next();
		else
			break;
		}
	while (true);

	expect(TokenType::RParen);
}

void WKTParser::parseGeometry()
{
	Token t = tokenizer.next();
	if (t.type != TokenType::Keyword)
		throw std::runtime_error("Expected geometry type");

	std::string g = t.text;

	if (g == "LINESTRING")
		parseLineString();
	else if (g == "POLYGON")
		parsePolygon();
	else if (g == "MULTILINESTRING")
		parseMultiLineString();
	else if (g == "MULTIPOLYGONE")
		parseMultiPolygon();
	else if (g == "GEOMETRYCOLLECTION")
		parseGeometryCollection();
	else
		throw std::runtime_error("Unsupported geometry: " + g);
}
