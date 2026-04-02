const vscode = require('vscode');
const fs = require('fs');
const os = require('os');
const path = require('path');

const MODULE_HEADER_RE = /^\s*\[\s*([A-Za-z0-9_+\-]+)\s*\]\s*$/;
const INLINE_BEGIN_RE = /^\s*#>>>\s+BANEWFN_INLINE_CONF_BEGIN\s+([A-Za-z0-9_+\-]+)\s*$/;

function activate(context) {
  const retagging = new Set();

  context.subscriptions.push(
    vscode.commands.registerCommand('banewfn.runCurrent', async (uri) => runCurrent(uri)),
    vscode.commands.registerCommand('banewfn.packCurrent', async (uri) => packCurrent(uri)),
    vscode.commands.registerCommand('banewfn.openRelatedConf', async (uri) => openRelatedConf(uri)),
    vscode.languages.registerDefinitionProvider(
      [{ language: 'banewfn' }],
      {
        provideDefinition(document, position) {
          return provideRelatedConfDefinition(document, position);
        }
      }
    ),
    vscode.workspace.onDidOpenTextDocument((document) => maybeRetagConfDocument(document, retagging)),
    vscode.window.onDidChangeActiveTextEditor((editor) => {
      if (editor && editor.document) {
        maybeRetagConfDocument(editor.document, retagging);
      }
    })
  );

  for (const document of vscode.workspace.textDocuments) {
    maybeRetagConfDocument(document, retagging);
  }
}

function deactivate() {}

function getConfig() {
  return vscode.workspace.getConfiguration('banewfn');
}

function getWorkspaceFolder(document) {
  return document ? vscode.workspace.getWorkspaceFolder(document.uri) : undefined;
}

function isBaneScriptDocument(document) {
  if (!document) {
    return false;
  }
  if (document.languageId === 'banewfn') {
    return true;
  }
  const ext = path.extname(document.uri.fsPath || '').toLowerCase();
  return ext === '.bw' || ext === '.bwc';
}

async function resolveDocumentFromUri(uri) {
  if (!uri) {
    return vscode.window.activeTextEditor ? vscode.window.activeTextEditor.document : undefined;
  }
  if (Array.isArray(uri) && uri.length > 0) {
    return resolveDocumentFromUri(uri[0]);
  }
  if (uri instanceof vscode.Uri) {
    try {
      const existing = vscode.workspace.textDocuments.find((doc) => doc.uri.toString() === uri.toString());
      return existing || await vscode.workspace.openTextDocument(uri);
    } catch {
      return undefined;
    }
  }
  return vscode.window.activeTextEditor ? vscode.window.activeTextEditor.document : undefined;
}

async function resolveScriptDocument(uri) {
  const document = await resolveDocumentFromUri(uri);
  if (!isBaneScriptDocument(document)) {
    vscode.window.showWarningMessage('Open a .bw or .bwc file first.');
    return undefined;
  }
  return document;
}

function getCwdForDocument(document) {
  const mode = getConfig().get('cwdMode', 'fileDir');
  const folder = getWorkspaceFolder(document);
  if (mode === 'workspaceRoot' && folder) {
    return folder.uri.fsPath;
  }
  if (document && document.uri.scheme === 'file') {
    return path.dirname(document.uri.fsPath);
  }
  return folder ? folder.uri.fsPath : undefined;
}

function splitArgs(text) {
  const input = String(text ?? '').trim();
  if (!input) {
    return [];
  }

  const args = [];
  let current = '';
  let quote = undefined;

  for (let i = 0; i < input.length; i += 1) {
    const ch = input[i];

    if (quote) {
      if (ch === quote) {
        quote = undefined;
      } else if (ch === '\\' && quote === '"' && i + 1 < input.length && (input[i + 1] === '"' || input[i + 1] === '\\')) {
        i += 1;
        current += input[i];
      } else {
        current += ch;
      }
      continue;
    }

    if (ch === '"' || ch === "'") {
      quote = ch;
      continue;
    }

    if (/\s/.test(ch)) {
      if (current) {
        args.push(current);
        current = '';
      }
      continue;
    }

    current += ch;
  }

  if (current) {
    args.push(current);
  }
  return args;
}

async function runProcessTask(taskLabel, executable, args, cwd, document) {
  const terminalName = getConfig().get('terminalName', 'BaneWfn');
  const scope = getWorkspaceFolder(document) || vscode.TaskScope.Global;
  const execution = new vscode.ProcessExecution(executable, args, cwd ? { cwd } : undefined);
  const task = new vscode.Task(
    { type: 'banewfn', task: taskLabel },
    scope,
    taskLabel,
    terminalName,
    execution,
    []
  );

  task.presentationOptions = {
    reveal: vscode.TaskRevealKind.Always,
    panel: vscode.TaskPanelKind.Dedicated,
    focus: true,
    clear: false,
    showReuseMessage: false
  };

  return vscode.tasks.executeTask(task);
}

function expandUser(inputPath) {
  if (!inputPath) {
    return inputPath;
  }
  if (inputPath === '~') {
    return os.homedir();
  }
  if (inputPath.startsWith('~/') || inputPath.startsWith(`~${path.sep}`)) {
    return path.join(os.homedir(), inputPath.slice(2));
  }
  return inputPath;
}

function resolveLocalPath(inputPath, document) {
  const expanded = expandUser(inputPath);
  if (!expanded) {
    return expanded;
  }
  if (path.isAbsolute(expanded)) {
    return expanded;
  }
  const workspaceFolder = getWorkspaceFolder(document);
  if (workspaceFolder) {
    return path.resolve(workspaceFolder.uri.fsPath, expanded);
  }
  if (document && document.uri.scheme === 'file') {
    return path.resolve(path.dirname(document.uri.fsPath), expanded);
  }
  return path.resolve(expanded);
}

function resolveCommandPath(commandOrPath, document) {
  if (!commandOrPath) {
    return commandOrPath;
  }
  if (/[\\/]/.test(commandOrPath)) {
    return resolveLocalPath(commandOrPath, document);
  }
  return commandOrPath;
}

async function runCurrent(uri) {
  const document = await resolveScriptDocument(uri);
  if (!document) {
    return;
  }

  const executablePath = resolveCommandPath(getConfig().get('executablePath', 'banewfn').trim() || 'banewfn', document);
  const extraArgs = splitArgs(getConfig().get('runExtraArgs', ''));
  await runProcessTask(`Run ${path.basename(document.uri.fsPath)}`, executablePath, [document.uri.fsPath, ...extraArgs], getCwdForDocument(document), document);
}

async function packCurrent(uri) {
  const document = await resolveScriptDocument(uri);
  if (!document) {
    return;
  }
  const ext = path.extname(document.uri.fsPath).toLowerCase();
  if (ext !== '.bw') {
    vscode.window.showWarningMessage('bwpack is intended for .bw source files.');
    return;
  }

  const bwpackPath = resolveCommandPath(getConfig().get('bwpackPath', 'bwpack').trim() || 'bwpack', document);
  const outputFile = replaceExtension(document.uri.fsPath, '.bwc');
  const confPath = getConfig().get('confPath', '').trim();
  const rcPath = getConfig().get('rcPath', '').trim();
  const args = [document.uri.fsPath, '--output', outputFile];

  if (confPath) {
    args.push('--confdir', resolveLocalPath(confPath, document));
  } else if (rcPath) {
    args.push('--rc', resolveLocalPath(rcPath, document));
  }

  await runProcessTask(`Pack ${path.basename(document.uri.fsPath)}`, bwpackPath, args, getCwdForDocument(document), document);
  vscode.window.setStatusBarMessage(`BaneWfn: packing to ${path.basename(outputFile)}`, 4000);
}

async function openRelatedConf(uri) {
  const document = await resolveScriptDocument(uri);
  if (!document) {
    return;
  }

  const moduleName = await pickModuleName(document, getCurrentPositionForDocument(document));
  if (!moduleName) {
    vscode.window.showWarningMessage('No module block found in the current BaneWfn script.');
    return;
  }

  const inlineLoc = findInlineConfLocation(document, moduleName);
  if (inlineLoc) {
    const editor = await vscode.window.showTextDocument(document, { preview: false });
    editor.selection = new vscode.Selection(inlineLoc.range.start, inlineLoc.range.start);
    editor.revealRange(inlineLoc.range, vscode.TextEditorRevealType.InCenter);
    return;
  }

  const externalUri = await findExternalConfUri(document, moduleName);
  if (!externalUri) {
    vscode.window.showWarningMessage(`Cannot find ${moduleName}.conf from current settings or workspace.`);
    return;
  }

  const opened = await vscode.workspace.openTextDocument(externalUri);
  await vscode.window.showTextDocument(opened, { preview: false });
}

async function provideRelatedConfDefinition(document, position) {
  const moduleName = await resolveModuleNameAtPosition(document, position);
  if (!moduleName) {
    return undefined;
  }

  const inlineLoc = findInlineConfLocation(document, moduleName);
  if (inlineLoc) {
    return inlineLoc;
  }

  const externalUri = await findExternalConfUri(document, moduleName);
  if (!externalUri) {
    return undefined;
  }
  return new vscode.Location(externalUri, new vscode.Position(0, 0));
}

function getCurrentPositionForDocument(document) {
  const editor = vscode.window.activeTextEditor;
  if (editor && editor.document.uri.toString() === document.uri.toString()) {
    return editor.selection.active;
  }
  return new vscode.Position(0, 0);
}

function findModuleNameOnLine(lineText) {
  const match = MODULE_HEADER_RE.exec(lineText);
  return match ? match[1] : undefined;
}

function listModulesInDocument(document) {
  const modules = [];
  const seen = new Set();
  for (let i = 0; i < document.lineCount; i += 1) {
    const moduleName = findModuleNameOnLine(document.lineAt(i).text);
    if (moduleName && !seen.has(moduleName)) {
      seen.add(moduleName);
      modules.push(moduleName);
    }
  }
  return modules;
}

async function resolveModuleNameAtPosition(document, position) {
  const direct = findModuleNameOnLine(document.lineAt(position.line).text);
  if (direct) {
    return direct;
  }

  const modules = listModulesInDocument(document);
  const wordRange = document.getWordRangeAtPosition(position, /[A-Za-z0-9_+\-]+/);
  if (wordRange) {
    const candidate = document.getText(wordRange);
    if (modules.includes(candidate)) {
      return candidate;
    }
  }
  return undefined;
}

async function pickModuleName(document, position) {
  const direct = await resolveModuleNameAtPosition(document, position);
  if (direct) {
    return direct;
  }

  const modules = listModulesInDocument(document);
  if (modules.length === 1) {
    return modules[0];
  }
  if (modules.length === 0) {
    return undefined;
  }
  return vscode.window.showQuickPick(modules, {
    placeHolder: 'Pick a module to open its related conf'
  });
}

function findInlineConfLocation(document, moduleName) {
  for (let i = 0; i < document.lineCount; i += 1) {
    const match = INLINE_BEGIN_RE.exec(document.lineAt(i).text);
    if (match && match[1] === moduleName) {
      return new vscode.Location(document.uri, new vscode.Position(i, 0));
    }
  }
  return undefined;
}

async function findExternalConfUri(document, moduleName) {
  const confDir = await resolveConfDir(document);
  const candidates = [];

  if (confDir) {
    candidates.push(path.join(confDir, `${moduleName}.conf`));
  }

  const workspaceFolder = getWorkspaceFolder(document);
  if (workspaceFolder) {
    candidates.push(path.join(workspaceFolder.uri.fsPath, 'conf', `${moduleName}.conf`));
    candidates.push(path.join(workspaceFolder.uri.fsPath, `${moduleName}.conf`));
  }

  for (const candidate of candidates) {
    if (candidate && fs.existsSync(candidate)) {
      return vscode.Uri.file(candidate);
    }
  }

  if (workspaceFolder) {
    const pattern = new vscode.RelativePattern(workspaceFolder, `**/${moduleName}.conf`);
    const found = await vscode.workspace.findFiles(pattern, '**/{.git,node_modules,build,dist}/**', 5);
    if (found.length > 0) {
      return found[0];
    }
  }

  return undefined;
}

async function resolveConfDir(document) {
  const explicitConfPath = getConfig().get('confPath', '').trim();
  if (explicitConfPath) {
    const resolved = resolveLocalPath(explicitConfPath, document);
    if (fs.existsSync(resolved)) {
      return resolved;
    }
  }

  const explicitRcPath = getConfig().get('rcPath', '').trim();
  if (explicitRcPath) {
    const confDir = parseConfPathFromRc(resolveLocalPath(explicitRcPath, document));
    if (confDir) {
      return confDir;
    }
  }

  const executablePath = resolveCommandPath(getConfig().get('executablePath', '').trim(), document);
  for (const rcCandidate of getRcCandidates(document, executablePath)) {
    const confDir = parseConfPathFromRc(rcCandidate);
    if (confDir) {
      return confDir;
    }
  }

  return undefined;
}

function getRcCandidates(document, executablePath) {
  const candidates = [];
  const seen = new Set();
  const push = (candidate) => {
    if (!candidate) {
      return;
    }
    const resolved = resolveLocalPath(candidate, document);
    if (!seen.has(resolved)) {
      seen.add(resolved);
      candidates.push(resolved);
    }
  };

  if (document && document.uri.scheme === 'file') {
    push(path.join(path.dirname(document.uri.fsPath), 'banewfn.rc'));
  }

  const workspaceFolder = getWorkspaceFolder(document);
  if (workspaceFolder) {
    push(path.join(workspaceFolder.uri.fsPath, 'banewfn.rc'));
  }

  if (executablePath && /[\\/]/.test(executablePath)) {
    push(path.join(path.dirname(executablePath), 'banewfn.rc'));
  }

  push(path.join(os.homedir(), '.bane', 'wfn', 'banewfn.rc'));
  return candidates;
}

function parseConfPathFromRc(rcPath) {
  if (!rcPath || !fs.existsSync(rcPath)) {
    return undefined;
  }
  try {
    const content = fs.readFileSync(rcPath, 'utf8');
    for (const rawLine of content.split(/\r?\n/)) {
      const line = stripInlineComment(rawLine).trim();
      if (!line) {
        continue;
      }
      const eq = line.indexOf('=');
      if (eq < 0) {
        continue;
      }
      const key = line.slice(0, eq).trim();
      const value = line.slice(eq + 1).trim();
      if (key === 'confpath' && value) {
        const expanded = expandUser(trimQuotes(value));
        return path.isAbsolute(expanded)
          ? expanded
          : path.resolve(path.dirname(rcPath), expanded);
      }
    }
  } catch {
    return undefined;
  }
  return undefined;
}

function stripInlineComment(line) {
  let result = '';
  let inSingle = false;
  let inDouble = false;

  for (let i = 0; i < line.length; i += 1) {
    const ch = line[i];
    const prev = i > 0 ? line[i - 1] : '';
    if (ch === '\'' && prev !== '\\' && !inDouble) {
      inSingle = !inSingle;
      result += ch;
      continue;
    }
    if (ch === '"' && prev !== '\\' && !inSingle) {
      inDouble = !inDouble;
      result += ch;
      continue;
    }
    if (ch === '#' && !inSingle && !inDouble) {
      break;
    }
    result += ch;
  }
  return result;
}

function trimQuotes(text) {
  if (!text) {
    return text;
  }
  if ((text.startsWith('"') && text.endsWith('"')) || (text.startsWith('\'') && text.endsWith('\''))) {
    return text.slice(1, -1);
  }
  return text;
}

function replaceExtension(filePath, newExt) {
  const parsed = path.parse(filePath);
  return path.join(parsed.dir, `${parsed.name}${newExt}`);
}

async function maybeRetagConfDocument(document, retagging) {
  if (!document || document.uri.scheme !== 'file') {
    return;
  }
  if (!getConfig().get('autoHighlightConfFiles', true)) {
    return;
  }
  if (document.languageId === 'banewfn-conf') {
    return;
  }
  if (path.extname(document.uri.fsPath).toLowerCase() !== '.conf') {
    return;
  }
  if (retagging.has(document.uri.toString())) {
    return;
  }

  const sample = document.getText();
  if (!looksLikeBaneConf(sample)) {
    return;
  }

  retagging.add(document.uri.toString());
  try {
    await vscode.languages.setTextDocumentLanguage(document, 'banewfn-conf');
  } catch {
    // ignore retag failure
  } finally {
    retagging.delete(document.uri.toString());
  }
}

function looksLikeBaneConf(text) {
  return /(^|\n)\s*\[main\]\s*($|\n)/m.test(text)
    || /(^|\n)\s*\[quit\]\s*($|\n)/m.test(text)
    || /(^|\n)\s*default\s*\{\s*($|\n)/m.test(text);
}

module.exports = {
  activate,
  deactivate
};
