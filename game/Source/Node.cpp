#include "DXUT.h"
#include "Node.h"

////////////////////////////////////////////////////////////////////////////


Node::Node(IDirect3DDevice9* /* pd3dDevice */)
{
}


Node::~Node()
{
	Unload();
}


HRESULT Node::Load(IDirect3DDevice9* /* pd3dDevice */, const LPCWSTR /* szFilename */)
{
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


HRESULT Node::GetInstance(const LPCWSTR /* szFilename */)
{
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void Node::Unload()
{
	// Recurse on children
	std::vector<std::tr1::shared_ptr<Node> >::const_iterator i;

	for (i = m_vecpChildren.begin(); i != m_vecpChildren.end(); ++i)
	{
//		delete (*i);
	}
}


void Node::AddChild(std::tr1::shared_ptr<Node> pNode)
{
	m_vecpChildren.push_back(pNode);
}


void Node::Update(double fTime)
{
	// Recurse on children
	std::vector<std::tr1::shared_ptr<Node> >::const_iterator i;

	for (i = m_vecpChildren.begin(); i != m_vecpChildren.end(); ++i)
	{
		(*i)->Update(fTime);
	}
}


void Node::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool shadowPass)
{
	// Recurse on children
	std::vector<std::tr1::shared_ptr<Node> >::const_iterator i;

	for (i = m_vecpChildren.begin(); i != m_vecpChildren.end(); ++i)
	{
		(*i)->Render(pd3dDevice, matWorld, shadowPass);
	}
}

////////////////////////////////////////////////////////////////////////////
