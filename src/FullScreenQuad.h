class CFullScreenQuad
{
public:
	CFullScreenQuad(void);
	~CFullScreenQuad(void);
	void initialize();
	void draw();
private:
	int m_iArraySize;
	float m_vArray[20];	//4 points of (x,y,z) = 12 floats + 4 points of (u,v) = 8 floats ==> 20 floats
	unsigned int m_uVBO;
	unsigned int m_uFBOId;
	unsigned int m_uRenderId;
	unsigned int m_uFBODepthId;
};

