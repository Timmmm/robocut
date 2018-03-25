#pragma once

class NoCopy
{
public:
	NoCopy() { }
	~NoCopy() { }
private:
	NoCopy(const NoCopy&) = delete;
	const NoCopy& operator=(const NoCopy&) = delete;
};

#define NOCOPY NoCopy nocopyvar
