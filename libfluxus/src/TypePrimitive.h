// Copyright (C) 2005 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef N_TYPEPRIM
#define N_TYPEPRIM

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Fluxus
{

//////////////////////////////////////////////////
/// TTF font primitive
class TypePrimitive : public Primitive
{
public:
	/// charw,h are in _texture_ coords not pixels
	TypePrimitive();
	TypePrimitive(const TypePrimitive &other);
	virtual ~TypePrimitive();

	///////////////////////////////////////////////////
	///@name Primitive Interface
	///@{
	virtual TypePrimitive* Clone() const;
	virtual void Render();
	virtual string GetTypeName() { return "TypePrimitive"; }
	virtual Evaluator *MakeEvaluator() { return NULL; }
	virtual void PDataDirty() {}
	virtual dBoundingBox GetBoundingBox(const dMatrix&) { return dBoundingBox(); }
	virtual void ApplyTransform(bool s) {}
	 ///@}

	bool LoadTTF(const string &FontFilename);
	void SetText(const string &s);
	void SetTextExtruded(const string &s, float depth);

	/// Fills supplied polygon primitive with the mesh
	/// (needs to be an empty triangle list)
	void ConvertToPoly(PolyPrimitive &poly);

protected:
	class GlyphGeometry
	{
	public:
		GlyphGeometry() {}
		~GlyphGeometry() {}

		class Mesh
		{
		public:
			Mesh(GLenum type) : m_Type(type) {}
			~Mesh() {}

			GLenum m_Type;
			vector<dVector> m_Positions;
			vector<dVector> m_Normals;
		};

		float m_Advance;
		dVector	m_Normal;
		GLenum m_Error;
		vector<Mesh> m_Meshes;
		vector<double *> m_CombinedData;
	};

	void Clear();
	void BuildGeometry(const FT_GlyphSlot &glyph, GlyphGeometry &geo, float depth, bool winding=true);
	void BuildExtrusion(const FT_GlyphSlot &glyph, GlyphGeometry &geo, float depth);
	void GenerateExtrusion(const FT_GlyphSlot &glyph, GlyphGeometry &geo, int from, int to, float depth);
	void RenderGeometry(const GlyphGeometry &geo);
	vector<GlyphGeometry*> m_GlyphVec;

	bool m_TTFLoaded;
	FT_Library    m_Library;
	FT_Face       m_Face;
	FT_GlyphSlot  m_Slot;


#ifndef WIN32
#define __stdcall
#endif

	static void __stdcall TessError(GLenum errCode, GlyphGeometry* geo);
	static void __stdcall TessVertex(void* data, GlyphGeometry* geo);
	static void __stdcall TessCombine(double coords[3], void *vertex_data[4], float weight[4], void** outData, GlyphGeometry* geo);
	static void __stdcall TessBegin(GLenum type, GlyphGeometry* geo);
	static void __stdcall TessEnd(GlyphGeometry* geo);

	static uint8_t const m_Trailing[256];
	static uint32_t const m_Offsets[6];
	uint32_t utf8_to_utf32(char const *s, size_t *bytes);
};

};

#endif

