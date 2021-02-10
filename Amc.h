#pragma once

class CAmc
{
public:
	CAmc(void);
	~CAmc(void);
public:
	int		BoardInit();
	void	DetectIO();
	void	On(short nNum);
   	void	Off(short nNum);

};
