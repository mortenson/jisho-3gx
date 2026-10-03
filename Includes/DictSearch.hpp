#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Prefix search over a sorted dictionary file.
//
// File format: one header line, then one entry per line, sorted bytewise.
// The search does a binary search for the first line that starts with the key,
// then reads forward while lines still match.
//
// `Reader` is a callable: size_t read(uint64_t offset, char *buffer, size_t length).
// It returns the number of bytes read, or 0 on error.
namespace Jisho
{
    namespace detail
    {
        // Reads through a small cache so nearby reads cost one file access.
        template <typename Reader>
        class Cursor
        {
        public:
            Cursor(Reader &reader, uint64_t size) : _reader(reader), _size(size) {}

            uint64_t Size() const { return _size; }

            bool At(uint64_t offset, char &out)
            {
                if (offset >= _size)
                    return false;
                if (offset < _bufOffset || offset >= _bufOffset + _bufLength)
                {
                    size_t want = static_cast<size_t>(std::min<uint64_t>(sizeof(_buffer), _size - offset));
                    size_t got = _reader(offset, _buffer, want);
                    if (got == 0)
                    {
                        _bufLength = 0;
                        return false;
                    }
                    _bufOffset = offset;
                    _bufLength = got;
                }
                out = _buffer[offset - _bufOffset];
                return true;
            }

            // Returns the start of the first line that begins after the byte at `offset`.
            uint64_t NextLineStart(uint64_t offset)
            {
                char c;
                while (At(offset, c))
                {
                    if (c == '\n')
                        return offset + 1;
                    ++offset;
                }
                return _size;
            }

            // Reads the line that starts at `offset`. Sets `next` to the start of the following line.
            bool ReadLine(uint64_t offset, std::string &line, uint64_t &next)
            {
                line.clear();
                char c;
                uint64_t i = offset;
                while (At(i, c))
                {
                    if (c == '\n')
                    {
                        next = i + 1;
                        return true;
                    }
                    line.push_back(c);
                    ++i;
                }
                next = i;
                return i > offset;
            }

        private:
            Reader &_reader;
            uint64_t _size;
            char _buffer[1024];
            uint64_t _bufOffset = 0;
            size_t _bufLength = 0;
        };

        inline bool StartsWith(const std::string &line, const std::string &key)
        {
            return line.compare(0, key.size(), key) == 0;
        }

        // True when the line sorts before every line that starts with `key`.
        inline bool SortsBefore(const std::string &line, const std::string &key)
        {
            return line.compare(0, key.size(), key) < 0;
        }
    }

    // Returns up to `maxResults` lines that start with `key`.
    template <typename Reader>
    std::vector<std::string> SearchPrefix(Reader reader, uint64_t fileSize, const std::string &key, size_t maxResults)
    {
        std::vector<std::string> results;
        if (key.empty() || maxResults == 0)
            return results;

        detail::Cursor<Reader> cursor(reader, fileSize);
        std::string line;

        // Skip the header line.
        uint64_t lo;
        if (!cursor.ReadLine(0, line, lo))
            return results;
        uint64_t hi = fileSize;

        // Invariant: `lo` is a line start, and every line before `lo` sorts before the key.
        // Every line that starts at or after `hi` does not sort before the key.
        while (lo < hi)
        {
            uint64_t mid = lo + (hi - lo) / 2;
            uint64_t lineStart = (mid == lo) ? lo : cursor.NextLineStart(mid - 1);
            if (lineStart >= hi)
            {
                hi = mid;
                continue;
            }
            uint64_t next;
            if (!cursor.ReadLine(lineStart, line, next))
                return results;
            if (detail::SortsBefore(line, key))
                lo = next;
            else
                hi = lineStart;
        }

        uint64_t offset = lo;
        uint64_t next;
        while (results.size() < maxResults && cursor.ReadLine(offset, line, next))
        {
            if (!detail::StartsWith(line, key))
                break;
            results.push_back(line);
            offset = next;
        }
        return results;
    }
}
