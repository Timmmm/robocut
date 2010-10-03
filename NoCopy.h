#pragma once

class NoCopy
{
public:
	NoCopy() { }
	~NoCopy() { }
private:
	NoCopy(const NoCopy&);
	const NoCopy& operator=(const NoCopy&);
};

#define NOCOPY NoCopy nocopyvar
