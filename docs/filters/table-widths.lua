-- Assign proportional widths to Markdown tables that do not declare widths.
-- Pandoc otherwise emits LaTeX l/c/r columns, whose cells cannot wrap.

local function visible_width(text)
  local longest = 0
  for line in (text .. "\n"):gmatch("(.-)\n") do
    local width = 0
    for _, codepoint in utf8.codes(line) do
      if codepoint < 0x80 then
        width = width + 1
      else
        -- CJK and most non-ASCII glyphs occupy roughly two Latin columns.
        width = width + 2
      end
    end
    if width > longest then longest = width end
  end
  return longest
end

local function visit_rows(rows, scores)
  for _, row in ipairs(rows or {}) do
    for index, cell in ipairs(row.cells or {}) do
      local text = pandoc.utils.stringify(cell.contents)
      local width = visible_width(text)
      if width > scores[index] then scores[index] = width end
    end
  end
end

function Table(table)
  if not FORMAT:match("latex") then return nil end

  local column_count = #table.colspecs
  if column_count == 0 then return nil end

  -- Respect widths explicitly supplied by the author.
  for _, spec in ipairs(table.colspecs) do
    if spec[2] and spec[2] > 0 then return nil end
  end

  local minimum_score = 10
  local maximum_score = 64
  if column_count >= 5 then
    minimum_score = 16
    maximum_score = 42
  elseif column_count == 4 then
    minimum_score = 14
    maximum_score = 50
  elseif column_count == 3 then
    minimum_score = 12
    maximum_score = 58
  end

  local scores = {}
  for index = 1, column_count do scores[index] = minimum_score end

  visit_rows(table.head and table.head.rows, scores)
  for _, body in ipairs(table.bodies or {}) do
    visit_rows(body.head, scores)
    visit_rows(body.body, scores)
  end
  visit_rows(table.foot and table.foot.rows, scores)

  local total = 0
  for index = 1, column_count do
    scores[index] = math.max(minimum_score, math.min(maximum_score, scores[index]))
    total = total + scores[index]
  end

  if total <= 0 then return nil end
  for index, spec in ipairs(table.colspecs) do
    table.colspecs[index] = {spec[1], scores[index] / total}
  end
  return table
end
