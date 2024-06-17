// Copyright (C) 2016 by rr-
//
// This file is part of arc_unpacker.
//
// arc_unpacker is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// arc_unpacker is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with arc_unpacker. If not, see <http://www.gnu.org/licenses/>.

#include "dec/ism/isa_archive_decoder.h"
#include "algo/range.h"
#include "io/memory_byte_stream.h"

using namespace au;
using namespace au::dec::ism;

namespace {
	enum class IsaVariant : u8
	{
		Variant1 = 1,
		Variant2,
	};
}

static const bstr magic = "ISM ARCHIVED"_b;

bool IsaArchiveDecoder::is_recognized_impl(io::File &input_file) const
{
    return input_file.stream.read(magic.size()) == magic;
}

static IsaVariant guess_isa_variant(io::BaseByteStream &input_stream)
{
	input_stream.seek(magic.size() + 2);
	const auto version = input_stream.read_le<u16>();
	if (version == 1)
		return IsaVariant::Variant1;

	return IsaVariant::Variant2;
}

std::unique_ptr<dec::ArchiveMeta> IsaArchiveDecoder::read_meta_impl(
    const Logger &logger, io::File &input_file) const
{
	const auto variant = guess_isa_variant(input_file.stream);
	const auto path_size = variant == IsaVariant::Variant1 ? 48 : 12;
	const auto skip_after_size = variant == IsaVariant::Variant1 ? 4 : 8;
	logger.info("detected ISA variant: %s.\n", variant == IsaVariant::Variant1 ? "1" : "2");

    input_file.stream.seek(magic.size());
    const auto file_count = input_file.stream.read_le<u16>();
    input_file.stream.skip(2);
    auto meta = std::make_unique<ArchiveMeta>();
    for (const auto i : algo::range(file_count))
    {
        auto entry = std::make_unique<PlainArchiveEntry>();
        entry->path = input_file.stream.read_to_zero(path_size).str();
        input_file.stream.skip(4);
        entry->offset = input_file.stream.read_le<u32>();
        entry->size = input_file.stream.read_le<u32>();
        input_file.stream.skip(skip_after_size);
        meta->entries.push_back(std::move(entry));
    }
    return meta;
}

std::unique_ptr<io::File> IsaArchiveDecoder::read_file_impl(
    const Logger &logger,
    io::File &input_file,
    const dec::ArchiveMeta &m,
    const dec::ArchiveEntry &e) const
{
    const auto entry = static_cast<const PlainArchiveEntry*>(&e);
    const auto data = input_file.stream.seek(entry->offset).read(entry->size);
    return std::make_unique<io::File>(entry->path, data);
}

std::vector<std::string> IsaArchiveDecoder::get_linked_formats() const
{
    return {"ism/isg"};
}

static auto _ = dec::register_decoder<IsaArchiveDecoder>("ism/isa");
