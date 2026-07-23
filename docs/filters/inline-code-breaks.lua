-- Insert invisible LaTeX line-break opportunities into long inline code.
-- A single \colorbox cannot span lines, so preamble.tex intentionally styles
-- inline code without a background box.

local break_after = {
  ["_"] = true, ["-"] = true, ["/"] = true, ["\\"] = true,
  ["."] = true, [":"] = true, ["="] = true, [","] = true,
  [";"] = true, ["|"] = true, ["?"] = true, ["&"] = true,
  ["+"] = true, [">"] = true, ["}"] = true, ["]"] = true,
  [")"] = true, [" "] = true,
}

local hard_chunk = 12

local function codepoints(text)
  local result = {}
  for _, codepoint in utf8.codes(text) do
    result[#result + 1] = utf8.char(codepoint)
  end
  return result
end

function Code(element)
  if not FORMAT:match("latex") then return nil end

  local chars = codepoints(element.text)
  if #chars <= hard_chunk then
    local has_natural_break = false
    for _, char in ipairs(chars) do
      if break_after[char] then
        has_natural_break = true
        break
      end
    end
    if not has_natural_break then return nil end
  end

  local pieces = {}
  local current = {}
  local since_break = 0

  local function flush()
    if #current == 0 then return end
    pieces[#pieces + 1] = table.concat(current)
    current = {}
    since_break = 0
  end

  for _, char in ipairs(chars) do
    current[#current + 1] = char
    since_break = since_break + 1
    if break_after[char] or since_break >= hard_chunk then flush() end
  end
  flush()

  if #pieces <= 1 then return nil end

  local output = {}
  for index, piece in ipairs(pieces) do
    local attr = index == 1 and element.attr or pandoc.Attr()
    output[#output + 1] = pandoc.Code(piece, attr)
    if index < #pieces then
      output[#output + 1] = pandoc.RawInline("latex", "\\allowbreak{}")
    end
  end
  return output
end
