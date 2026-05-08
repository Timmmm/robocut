#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

template <typename T> class OkVal;
template <typename E> class ErrVal;

// Result is a Rust-like class that provides an Ok() or Err() state.
template <typename T, typename E> class Result : private std::variant<T, E>
{
	// Inherit constructors.
	using std::variant<T, E>::variant;

	friend class OkVal<T>;
	friend class ErrVal<E>;

public:
	// Return the Ok value or throw an exception if it isn't ok.
	T& unwrap()
	{
		return expect("Called unwrap() on an Err Result.");
	}

	// Return the Ok value or throw an exception with the specified message.
	T& expect(std::string_view message)
	{
		if (!is_ok())
			throw std::runtime_error(std::string(message));
		return std::get<0>(*this);
	}

	// Return the Err value, or throw an exception if this isn't an error.
	E& unwrap_err()
	{
		if (!is_err())
			throw std::runtime_error("Called unwrap_err() on an Ok Result.");
		return std::get<1>(*this);
	}

	// Get the value or the default value.
	T unwrap_or_default()
	{
		return is_ok() ? std::get<0>(*this) : T();
	}

	// Get the value or an alternative value if it is an error.
	T& unwrap_or(T& val)
	{
		return is_ok() ? std::get<0>(*this) : val;
	}

	// If this result is Err() you can convert it to another result type.
	template <typename T2> operator Result<T2, E>()
	{
		if (!is_err())
			throw std::runtime_error("Error converting Result types - Result was Ok!");

		return Result<T2, E>(std::in_place_index_t<1>(), std::get<1>(*this));
	}

	// void version.
	operator Result<void, E>()
	{
		if (!is_err())
			throw std::runtime_error("Error converting Result types - Result was Ok!");

		return Result<void, E>(std::in_place_index_t<1>(), std::get<1>(*this));
	}

	bool is_ok() const
	{
		return std::variant<T, E>::index() == 0;
	}

	bool is_err() const
	{
		return std::variant<T, E>::index() != 0;
	}

	operator bool() const
	{
		return is_ok();
	}

	bool operator!() const
	{
		return is_err();
	}
};

// Specialisation for void Ok type because std::variant cannot contain a `void` type.
template <typename E> class Result<void, E> : private std::variant<std::monostate, E>
{
	using std::variant<std::monostate, E>::variant;

	friend class OkVal<void>;
	friend class ErrVal<E>;

public:
	void unwrap()
	{
		expect("Error unwrapping value!");
	}

	void expect(const std::string& message)
	{
		if (!is_ok())
		{
			throw std::runtime_error(message);
		}
	}

	// Return the Err value, or throw an exception if this isn't an error.
	E& unwrap_err()
	{
		if (!is_err())
		{
			throw std::runtime_error("Called unwrap_err() on an Ok Result.");
		}
		return std::get<1>(*this);
	}

	// If this result is Err() you can convert it to another result type.
	template <typename T2> operator Result<T2, E>()
	{
		if (!is_err())
		{
			throw std::runtime_error("Error converting Result types - Result was Ok!");
		}

		return Result<T2, E>(std::in_place_index_t<1>(), std::get<1>(*this));
	}

	// void version not needed because it's already that type.

	bool is_ok() const
	{
		return std::variant<std::monostate, E>::index() == 0;
	}

	bool is_err() const
	{
		return std::variant<std::monostate, E>::index() != 0;
	}

	operator bool() const
	{
		return is_ok();
	}

	bool operator!() const
	{
		return is_err();
	}
};

// These allow you to `return Ok(42);`.
template <typename T> class OkLVal
{
public:
	OkLVal(const T& val) : value(val)
	{
	}

	template <typename E> operator Result<T, E>() const
	{
		return Result<T, E>(std::in_place_index_t<0>(), value);
	}

private:
	const T& value;
};

template <typename T> class OkRVal
{
public:
	OkRVal(T&& val) : value(std::move(val))
	{
	}

	template <typename E> operator Result<T, E>() &&
	{
		return Result<T, E>(std::in_place_index_t<0>(), std::move(value));
	}

private:
	T&& value;
};

// Specialisation for void.
class OkVoid
{
public:
	template <typename E> operator Result<void, E>() const
	{
		return Result<void, E>();
	}
};

// T can be inferred because it is in a parameter list.
template <typename T> OkLVal<T> Ok(const T& val)
{
	return OkLVal<T>(val);
}

template <typename T> OkRVal<T> Ok(T&& val)
{
	return OkRVal<T>(std::move(val));
}

// Specialisation for void.
inline OkVoid Ok()
{
	return OkVoid();
}

// The same for errors.
template <typename E> class ErrLVal
{
public:
	ErrLVal(const E& err) : error(err)
	{
	}

	template <typename T> operator Result<T, E>() const
	{
		return Result<T, E>(std::in_place_index_t<1>(), error);
	}

private:
	const E& error;
};

template <typename E> class ErrRVal
{
public:
	ErrRVal(E&& err) : error(std::move(err))
	{
	}

	template <typename T> operator Result<T, E>() &&
	{
		return Result<T, E>(std::in_place_index_t<1>(), std::move(error));
	}

private:
	E&& error;
};

// T can be inferred because it is in a parameter list.
template <typename E> ErrLVal<E> Err(const E& err)
{
	return ErrLVal<E>(err);
}

template <typename E> ErrRVal<E> Err(E&& err)
{
	return ErrRVal<E>(std::move(err));
}

// Like Rust's try!(). Only works on GCC or Clang because it uses statement expressions.
// See https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html

#if defined(__clang__) || defined(__GNUC__)

#define TRY(x) \
	({ \
		auto&& ref = (x); \
		if (!ref) \
		{ \
			return ref; \
		} \
		ref.unwrap(); \
	})

#else

// For MSVC: TRY() where you don't need to use the result.
#define TRY(x) \
	do \
	{ \
		auto&& ref = (x); \
		if (!ref) \
		{ \
			return ref; \
		} \
	} while (0)

// For MSVC: TRY() where you want to assign the result to a variable.
#define TRY_ASSIGN(o, x) \
	do \
	{ \
		auto&& ref = (x); \
		if (!ref) \
		{ \
			return ref; \
		} \
		o = ref.unwrap(); \
	} while (0)

#endif

// Result with a std::string error type for convenience.
template <typename T = void> using SResult = Result<T, std::string>;
