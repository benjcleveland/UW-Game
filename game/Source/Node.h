#pragma once
#include <vector>

////////////////////////////////////////////////////////////////////////////


class Node
{
public:
						Node(IDirect3DDevice9* pd3dDevice);
	virtual				~Node();

	virtual HRESULT		Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename);
	virtual HRESULT		GetInstance(const LPCWSTR szFilename);
	virtual void		Unload();

	// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool shadowPass);

						// Hierarchy management
	int					GetNumChildren()			{return m_vecpChildren.size();}
	std::tr1::shared_ptr<Node> GetChild(int iChild)	{return m_vecpChildren[iChild];}
	void				AddChild(std::tr1::shared_ptr<Node> pNode);

protected:
	std::vector<std::tr1::shared_ptr<Node> >	m_vecpChildren;
};


////////////////////////////////////////////////////////////////////////////
