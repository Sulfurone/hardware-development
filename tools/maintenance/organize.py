"""Organize the local collection without changing paths inside projects."""
from pathlib import Path
import csv
import json
import subprocess
import re
import zipfile
import logging
from concurrent.futures import ThreadPoolExecutor
logging.getLogger('pypdf').setLevel(logging.ERROR)

ROOT = Path(__file__).resolve().parents[2]
GROUPS = {
    '01-projects': ['毕设', '课设', '茶杯', '晶体管测量仪'],
    '02-competitions': ['电赛2022（循迹小车）', '电赛2023（同轴电缆）', '电赛2024（晶体管测量仪）', '电赛电源题国赛资料'],
    '03-boards-and-examples': ['51环境', '精英mini', '普中32f407', '野火f103ve', '正点原子103zet6', 'GD', 'STCzhufeiku', 'STM32入门教程资料'],
    '04-modules': ['ESP8266固件烧录资料', 'mini平衡', 'mpu9250全部'],
    '05-reference': ['461单片机课程设计-先转存在提取以免丢失', '六位半资料.rar', '模拟电路讲义(2).pdf', '嵌入式系统高级C语言编程_12778630.pdf', '全元素.zip', '十大滤波.rar', '西电180903PCB培训.pdf', '信号处理中相关法的应用.pptx', '信号发生器.zip', '一种由555时基电路构成的多谐振荡电路 (2).pdf', '硬件工程师入门教程(3).pdf', '运放的单电源供电.pdf', '运算放大器权威指南 (第3版)  pdf (2).pdf', 'STC15_LIB-master.zip'],
    '06-local-tools': ['keil 5', 'STC烧录', 'XCOM V2.6', '串口调试助手.zip', 'CH34x_Install_Windows_v3_4.EXE', 'Proteus8.17 SP2.rar', 'Setup_JLink_V498e.exe', 'UartAssist.zip', 'USB转串口CH340驱动安装包.zip'],
}

IGNORE = '''# Local audit reports and installed software
/.local-audit/
/06-local-tools/
/01-projects/晶体管测量仪/tools/
# Archives, videos, installers and packaged dependencies
*.zip
*.rar
*.7z
*.tar
*.gz
*.exe
*.msi
*.apk
*.ipa
*.msix
*.msixbundle
*.iso
*.img
*.lnk
*.url
*.dll
*.sys
*.cab
*.pack
*.mp4
*.avi
*.mov
*.wmv
*.mkv
*.flv
*.swf
*.mpg
*.mpeg
*.rmvb
*.rm
*.asf
*.mp3
*.wav
# Build outputs and editor state
[Oo]bjects/
[Oo]bject/
[Oo]bj/
[Ll]istings/
[Ll]isting/
[Dd]ebug/
[Rr]elease/
[Bb]uild/
[Oo]utput/
[Oo]utput[sS]/
__pycache__/
node_modules/
.venv/
.cache/
.idea/
.vs/
*.o
*.obj
*.a
*.aem4f
*.pem4f
*.bin_rep
*.lib
*.axf
*.elf
*.out
*.sim
*.pbi
*.r51
*.r90
*.r79
*.r43
*.d43
*.d51
*.d90
*.sbr
*.tlog
*.browse
*.ewt
*.hex
*.bin
*.map
*.lst
*.m51
*.plg
*.lnp
*.d
*.dep
*.crf
*.iex
*.i
*.__i
*.sct.bak
*.bak
*.bkp
*.tmp
*.log
*.pyc
*.uvgui*
*.uvopt*
*.pwi
*.workspace
*.dbk
*.sdf
*.suo
*.ncb
*.bsc
*.pch
*.idb
*.pdb
*.ilk
*.ipch
*.ilk
*_build_log.htm
~$*
Thumbs.db
desktop.ini
# Local credentials and machine configuration
.env
.env.*
*.pem
*.key
*.pfx
*.p12
.mcp.json
.claude/
.codex/
'''

PRIVATE_NAME = re.compile(r'学籍|身份证|登记表|报名表|证件|成绩单|通讯录|访问密码|解压密码|keygen|破解|注册机|Security copy', re.I)
SECRET = re.compile(r'gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{30,}|sk-[A-Za-z0-9]{30,}|AKIA[A-Z0-9]{16}|-----BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY-----')
PERSONAL = re.compile(r'(?<!\d)[1-9]\d{5}(?:19|20)\d{2}(?:0[1-9]|1[0-2])(?:0[1-9]|[12]\d|3[01])\d{3}[\dXx](?!\d)|(?:身份证|手机号码?|联系电话|学号|联系电话|邮箱)\s*[:：]\s*\S{5,}')

def git(*args, **kwargs):
    return subprocess.run(['git', '-c', 'core.quotepath=false', *args], cwd=ROOT, check=True, capture_output=True, **kwargs)

def main():
    audit = ROOT / '.local-audit'
    audit.mkdir(exist_ok=True)
    moves = []
    for group, names in GROUPS.items():
        (ROOT / group).mkdir(exist_ok=True)
        for name in names:
            src, dst = ROOT / name, ROOT / group / name
            if src.exists():
                if dst.exists():
                    raise RuntimeError(f'Destination already exists: {dst}')
                # Both paths are resolved and checked before moving a directory.
                if not src.resolve().is_relative_to(ROOT) or not dst.resolve().is_relative_to(ROOT):
                    raise RuntimeError('Path outside workspace')
                src.rename(dst)
            if dst.exists():
                moves.append({'original': name, 'current': f'{group}/{name}'})
    (audit / 'moves.json').write_text(json.dumps(moves, ensure_ascii=False, indent=2), encoding='utf-8')
    (ROOT / '.gitignore').write_text(IGNORE, encoding='utf-8')
    if not (ROOT / '.git').exists():
        git('init', '-b', 'main')
    git('config', 'core.ignorecase', 'true')
    git('config', 'core.longpaths', 'true')
    git('config', 'core.autocrlf', 'false')
    candidates = git('ls-files', '--others', '--exclude-standard', '-z').stdout.decode('utf-8').split('\0')
    excluded = []
    docs_count = 0
    def inspect(rel):
        if not rel:
            return None
        p = ROOT / rel
        reason = None
        if not p.is_file():
            reason = 'embedded-repository'
        elif PRIVATE_NAME.search(rel):
            reason = 'personal-document-or-local-only-material'
        elif p.stat().st_size >= 95 * 1024 * 1024:
            reason = 'large-file'
        elif not p.suffix and p.stat().st_size > 0:
            # Keil produces extensionless ELF/object artifacts.
            if p.open('rb').read(4) == b'\x7fELF':
                reason = 'extensionless-ELF-build-output'
        elif p.suffix.lower() in {'.c', '.h', '.ino', '.py', '.json', '.md', '.txt', '.ini', '.yaml', '.yml', '.xml', '.cpp', '.js'}:
            if p.stat().st_size < 10 * 1024 * 1024:
                content = p.read_text(encoding='utf-8', errors='ignore')
                if SECRET.search(content):
                    reason = 'credential-pattern'
                elif p.suffix.lower() in {'.txt', '.md'} and PERSONAL.search(content):
                    reason = 'personal-data-pattern'
        elif p.suffix.lower() in {'.docx', '.pptx'}:
            try:
                with zipfile.ZipFile(p) as z:
                    body = ' '.join(re.sub('<[^>]+>', '', z.read(n).decode('utf-8', errors='ignore')) for n in z.namelist() if n.endswith('.xml') and (n.startswith('word/') or n.startswith('ppt/slides/')))
                    if PERSONAL.search(body) or SECRET.search(body):
                        reason = 'personal-data-in-document'
            except (zipfile.BadZipFile, OSError):
                reason = 'unreadable-document'
        elif p.suffix.lower() == '.doc':
            body = p.read_bytes().decode('utf-16le', errors='ignore')
            if PERSONAL.search(body) or SECRET.search(body):
                reason = 'personal-data-in-document'
        elif p.suffix.lower() == '.pdf':
            try:
                from pypdf import PdfReader
                reader = PdfReader(p)
                # Names and title pages are screened; large technical manuals
                # are sampled to avoid parsing thousands of pages per copy.
                for page in list(reader.pages)[:15]:
                    body = page.extract_text() or ''
                    if PERSONAL.search(body) or SECRET.search(body):
                        reason = 'personal-data-in-document'
                        break
            except Exception:
                reason = 'unreadable-document'
        return {'path': rel, 'reason': reason} if reason else None
    docs_count = sum(Path(r).suffix.lower() in {'.doc', '.docx', '.pdf', '.pptx'} for r in candidates)
    with ThreadPoolExecutor(max_workers=16) as pool:
        for index, result in enumerate(pool.map(inspect, candidates)):
            if result:
                excluded.append(result)
            if index % 500 == 0:
                (audit / 'progress.json').write_text(json.dumps({'index': index, 'total': len(candidates)}, ensure_ascii=False), encoding='utf-8')
    with (ROOT / '.gitignore').open('a', encoding='utf-8') as f:
        f.write('\n# Files withheld by publication audit (kept locally)\n')
        for item in excluded:
            escaped = re.sub(r'([\[\]*?!])', r'\\\1', item['path'])
            f.write('/' + escaped + '\n')
    (audit / 'excluded.json').write_text(json.dumps(excluded, ensure_ascii=False, indent=2), encoding='utf-8')
    selected = [r for r in git('ls-files', '--others', '--exclude-standard', '-z').stdout.decode('utf-8').split('\0') if r]
    total = sum((ROOT / r).stat().st_size for r in selected)
    summary = {'selected_files': len(selected), 'selected_MB': round(total / 1024**2, 2), 'audit_excluded_files': len(excluded), 'office_documents_scanned': docs_count}
    (audit / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    with (audit / 'all-files.csv').open('w', encoding='utf-8-sig', newline='') as f:
        w = csv.writer(f)
        w.writerow(['path', 'bytes', 'upload'])
        chosen = set(selected)
        for p in ROOT.rglob('*'):
            if p.is_file() and '.git' not in p.parts and '.local-audit' not in p.parts:
                r = p.relative_to(ROOT).as_posix()
                w.writerow([r, p.stat().st_size, r in chosen])
    print(json.dumps(summary, ensure_ascii=False))

if __name__ == '__main__':
    main()
