"""Generate the public catalog from files selected for the Git index."""
from pathlib import Path
from collections import defaultdict
from urllib.parse import quote
import subprocess

ROOT = Path(__file__).resolve().parents[2]

def main():
    paths = subprocess.run(['git', 'ls-files', '--cached', '--others', '--exclude-standard', '-z'], cwd=ROOT, check=True, capture_output=True).stdout.decode('utf-8').split('\0')
    groups = defaultdict(lambda: [0, 0])
    for rel in set(paths):
        if not rel or not rel[:2].isdigit():
            continue
        p = ROOT / rel
        if not p.is_file():
            continue
        parts = Path(rel).parts
        group = '/'.join(parts[:2])
        groups[group][0] += 1
        groups[group][1] += p.stat().st_size
    rows = ['# 资料目录', '', '统计范围为公开仓库中的文件，不包含本地保留的安装包、压缩包、视频、编译产物和个人资料。', '', '| 分类 / 资料 | 文件数 | 原始大小（MiB） |', '| --- | ---: | ---: |']
    for name, (count, size) in sorted(groups.items()):
        url = '../' + quote(name, safe='/')
        rows.append(f'| [{name}]({url}) | {count:,} | {size / 1024**2:.2f} |')
    rows.extend(['', f'资料合计：{sum(v[0] for v in groups.values()):,} 个文件，{sum(v[1] for v in groups.values()) / 1024**2:.2f} MiB。', '', '部分资料只有压缩包，没有已解压文件，因此仅保存在本地，未列入此表。'])
    (ROOT / 'docs').mkdir(exist_ok=True)
    (ROOT / 'docs/catalog.md').write_text('\n'.join(rows) + '\n', encoding='utf-8')
    print(f'Catalog generated: {len(groups)} entries')

if __name__ == '__main__':
    main()
