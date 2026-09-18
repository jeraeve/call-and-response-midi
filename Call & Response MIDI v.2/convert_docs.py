#!/usr/bin/env python3
import re
import html
import subprocess
import os

def md_to_html_body(md_text):
    lines = md_text.splitlines()
    html_lines = []
    in_list = False
    in_table = False
    table_headers = []
    in_code = False
    code_lines = []
    
    for line in lines:
        # Code blocks
        if line.strip().startswith("```"):
            if in_code:
                in_code = False
                html_lines.append(f"<pre><code>{html.escape(chr(10).join(code_lines))}</code></pre>")
                code_lines = []
            else:
                if in_list:
                    html_lines.append("</ul>")
                    in_list = False
                if in_table:
                    html_lines.append("</tbody></table>")
                    in_table = False
                in_code = True
                code_lines = []
            continue
        
        if in_code:
            code_lines.append(line)
            continue
            
        stripped = line.strip()
        if not stripped:
            if in_list:
                html_lines.append("</ul>")
                in_list = False
            if in_table:
                html_lines.append("</tbody></table>")
                in_table = False
            continue
            
        # Horizontal rule
        if stripped == "---":
            if in_list:
                html_lines.append("</ul>")
                in_list = False
            if in_table:
                html_lines.append("</tbody></table>")
                in_table = False
            html_lines.append("<hr>")
            continue
            
        # Headers
        if stripped.startswith("# "):
            if in_list: html_lines.append("</ul>"); in_list = False
            if in_table: html_lines.append("</tbody></table>"); in_table = False
            html_lines.append(f"<h1>{inline_fmt(stripped[2:])}</h1>")
            continue
        if stripped.startswith("## "):
            if in_list: html_lines.append("</ul>"); in_list = False
            if in_table: html_lines.append("</tbody></table>"); in_table = False
            html_lines.append(f"<h2>{inline_fmt(stripped[3:])}</h2>")
            continue
        if stripped.startswith("### "):
            if in_list: html_lines.append("</ul>"); in_list = False
            if in_table: html_lines.append("</tbody></table>"); in_table = False
            html_lines.append(f"<h3>{inline_fmt(stripped[4:])}</h3>")
            continue
        if stripped.startswith("#### "):
            if in_list: html_lines.append("</ul>"); in_list = False
            if in_table: html_lines.append("</tbody></table>"); in_table = False
            html_lines.append(f"<h4>{inline_fmt(stripped[5:])}</h4>")
            continue
            
        # Table
        if stripped.startswith("|") and stripped.endswith("|"):
            cells = [c.strip() for c in stripped[1:-1].split("|")]
            if all(set(c).issubset({'-', ':', ' '}) for c in cells):
                continue # separator
            if not in_table:
                if in_list: html_lines.append("</ul>"); in_list = False
                in_table = True
                html_lines.append("<table><thead><tr>")
                for c in cells:
                    html_lines.append(f"<th>{inline_fmt(c)}</th>")
                html_lines.append("</tr></thead><tbody>")
            else:
                html_lines.append("<tr>")
                for c in cells:
                    html_lines.append(f"<td>{inline_fmt(c)}</td>")
                html_lines.append("</tr>")
            continue
        else:
            if in_table:
                html_lines.append("</tbody></table>")
                in_table = False

        # Lists
        if stripped.startswith("- ") or stripped.startswith("* "):
            if not in_list:
                in_list = True
                html_lines.append("<ul>")
            html_lines.append(f"<li>{inline_fmt(stripped[2:])}</li>")
            continue
        elif in_list:
            html_lines.append("</ul>")
            in_list = False

        # Regular paragraphs
        html_lines.append(f"<p>{inline_fmt(stripped)}</p>")

    if in_list:
        html_lines.append("</ul>")
    if in_table:
        html_lines.append("</tbody></table>")
    if in_code:
        html_lines.append(f"<pre><code>{html.escape(chr(10).join(code_lines))}</code></pre>")

    return "\n".join(html_lines)

def inline_fmt(text):
    # escape basic html
    # But preserve formatting
    t = text
    # bold italic
    t = re.sub(r'\*\*\*(.*?)\*\*\*', r'<strong><em>\1</em></strong>', t)
    # bold
    t = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', t)
    # italic
    t = re.sub(r'\*(.*?)\*', r'<em>\1</em>', t)
    # code
    t = re.sub(r'`(.*?)`', r'<code>\1</code>', t)
    # math
    t = re.sub(r'\$(.*?)\$', r'<code>\1</code>', t)
    return t

HTML_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{title}</title>
<style>
  :root {{
    --bg: #0f1117;
    --card-bg: #181b24;
    --border: #2a2e3d;
    --text: #e2e8f0;
    --text-muted: #94a3b8;
    --cyan: #00d2ff;
    --cyan-glow: rgba(0, 210, 255, 0.15);
    --orange: #ff9f43;
    --code-bg: #0a0c10;
    --table-stripe: #1e2230;
  }}
  * {{ box-sizing: border-box; margin: 0; padding: 0; }}
  body {{
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
    background: var(--bg);
    color: var(--text);
    line-height: 1.6;
    padding: 40px 20px;
  }}
  .container {{
    max-width: 880px;
    margin: 0 auto;
    background: var(--card-bg);
    border: 1px solid var(--border);
    border-radius: 12px;
    padding: 48px;
    box-shadow: 0 10px 40px rgba(0,0,0,0.5);
  }}
  h1 {{
    color: #fff;
    font-size: 2.1rem;
    margin-bottom: 12px;
    border-bottom: 2px solid var(--cyan);
    padding-bottom: 10px;
    break-after: avoid;
  }}
  h2 {{
    color: #fff;
    font-size: 1.45rem;
    margin-top: 32px;
    margin-bottom: 14px;
    border-left: 4px solid var(--cyan);
    padding-left: 12px;
    break-after: avoid;
  }}
  h3 {{
    color: var(--cyan);
    font-size: 1.15rem;
    margin-top: 22px;
    margin-bottom: 8px;
    break-after: avoid;
  }}
  h4 {{
    color: var(--orange);
    font-size: 1.02rem;
    margin-top: 16px;
    margin-bottom: 6px;
    break-after: avoid;
  }}
  p {{
    margin-bottom: 14px;
    color: var(--text);
  }}
  strong {{
    color: #ffffff;
    font-weight: 600;
  }}
  hr {{
    border: 0;
    height: 1px;
    background: var(--border);
    margin: 28px 0;
  }}
  ul {{
    margin-left: 24px;
    margin-bottom: 16px;
  }}
  li {{
    margin-bottom: 6px;
  }}
  code {{
    background: var(--code-bg);
    color: var(--cyan);
    padding: 2px 6px;
    border-radius: 4px;
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
    font-size: 0.88em;
    border: 1px solid rgba(0,210,255,0.2);
  }}
  pre {{
    background: var(--code-bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 16px 20px;
    overflow-x: auto;
    margin-bottom: 20px;
  }}
  pre code {{
    background: transparent;
    border: none;
    padding: 0;
    color: #a5b4fc;
    font-size: 0.9em;
    display: block;
    line-height: 1.5;
  }}
  table {{
    width: 100%;
    border-collapse: collapse;
    margin: 20px 0;
    border: 1px solid var(--border);
    border-radius: 6px;
    overflow: hidden;
    font-size: 0.93rem;
  }}
  th {{
    background: #222736;
    color: var(--cyan);
    text-align: left;
    padding: 12px 16px;
    border-bottom: 2px solid var(--border);
    font-weight: 600;
  }}
  td {{
    padding: 10px 16px;
    border-bottom: 1px solid var(--border);
    vertical-align: top;
  }}
  tr:nth-child(even) {{
    background: var(--table-stripe);
  }}
  tr:last-child td {{
    border-bottom: none;
  }}
  @media print {{
    body {{ background: #fff; color: #111; padding: 0; }}
    .container {{ background: #fff; border: none; box-shadow: none; padding: 20px; max-width: 100%; }}
    h1 {{ color: #000; border-bottom: 2px solid #0099cc; }}
    h2 {{ color: #111; border-left: 4px solid #0099cc; }}
    h3 {{ color: #0077aa; }}
    h4 {{ color: #cc6600; }}
    strong {{ color: #000; }}
    th {{ background: #f0f4f8; color: #0077aa; border-bottom: 2px solid #ccc; }}
    tr:nth-child(even) {{ background: #f9fbfd; }}
    td, table {{ border-color: #ddd; }}
    code {{ background: #f1f5f9; color: #0077aa; border-color: #cbd5e1; }}
    pre {{ background: #f8fafc; border-color: #cbd5e1; }}
    pre code {{ color: #334155; }}
  }}
</style>
</head>
<body>
<div class="container">
{content}
</div>
</body>
</html>
"""

def generate_docs():
    base_dir = "/Volumes/Dock HD/Antigravity Projects/Call & Response MIDI/Call & Response MIDI v.2"
    doc_dir = os.path.join(base_dir, "Documentation")
    os.makedirs(doc_dir, exist_ok=True)
    
    # 1. User Manual
    manual_md_path = os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.md")
    with open(manual_md_path, "r", encoding="utf-8") as f:
        manual_md = f.read()
    
    manual_body = md_to_html_body(manual_md)
    manual_html = HTML_TEMPLATE.format(title="Call & Response MIDI — User Manual (v2.0.0)", content=manual_body)
    
    manual_html_path1 = os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.html")
    manual_html_path2 = os.path.join(doc_dir, "Call_and_Response_MIDI_User_Manual.html")
    manual_html_path3 = os.path.join(doc_dir, "User_Manual.html")
    
    for p in [manual_html_path1, manual_html_path2, manual_html_path3]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(manual_html)
    print("Generated User Manual HTMLs.")

    # 2. Read Me
    readme_md_path = os.path.join(base_dir, "Read Me.md")
    with open(readme_md_path, "r", encoding="utf-8") as f:
        readme_md = f.read()
        
    readme_body = md_to_html_body(readme_md)
    readme_html = HTML_TEMPLATE.format(title="Call & Response MIDI — Read Me (v2.0.0)", content=readme_body)
    
    readme_html_path1 = os.path.join(base_dir, "Read_Me.html")
    readme_html_path2 = os.path.join(doc_dir, "Read_Me.html")
    for p in [readme_html_path1, readme_html_path2]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(readme_html)
    print("Generated Read Me HTMLs.")

    # 3. Read Me.txt
    txt_content = f"""======================================================================
CALL & RESPONSE MIDI (v2.0.0)
Viral Samples | Intelligent Algorithmic Phrase Generator & 303 Sequencer
======================================================================

Formats: VST3, Audio Unit (AU), Standalone
Platforms: macOS Universal (Mojave 10.14+ Intel & Apple Silicon) & Windows 64-bit

----------------------------------------------------------------------
ATTRIBUTIONS & ACKNOWLEDGEMENTS
----------------------------------------------------------------------
Call & Response MIDI stands on the shoulders of creative musical innovators:
- Nordl3: Original project concept and algorithmic dual-phrase dialogue.
- SideBrain: Renowned Max for Live implementation and workflow concepts.

EVOLUTION BY VIRAL SAMPLES (v2.0.0):
- Complete modern dark interface overhaul with fixed 1180x750 aspect ratio.
- Interactive top ribbon bar with direct value displays for OCT, KEY,
  SCALE, b/#, STEPS, RATE, SWING with dropdown menus & steppers.
- Tempo-relative RATE: Half speed, 0 (Normal 16th sync), Double speed.
- Continuous SWING (0%-100%) and VELOCITY (1-127) vertical drag editing.
- Dual Phrase Lanes (CALL & RESPONSE) with independent MUTE bypass,
  DICE mutators, modifiable MUTATE % depth, and Lane Lock padlocks.
- Four-Tab Inspector Panel: GENERATE, SHAPE, ARTICULATION, OPTIONS with
  individual vector parameter locks for procedural composition.
- Interactive Piano Roll with color-coded notes (Orange for Call, Teal
  for Response), repeatable right-edge gate resizing up to 4.0x, and
  step auditioning with auto-stop on mouse release.
- Integrated analog-modelled 303 preview synthesizer with resonant low-pass
  filter and snappy envelope decay.
- 8 rapid-access preset slots [A]-[H] (Cmd+Click save, double-click load).
- Multi-level undo/redo history and global All-Notes-Off MIDI Panic.
- Drag & drop MIDI export directly to DAW arrangements.

----------------------------------------------------------------------
INSTALLATION
----------------------------------------------------------------------
macOS Universal (.pkg):
Double-click "Call & Response MIDI v2.0.0 Installer.pkg" to install:
  - VST3: /Library/Audio/Plug-Ins/VST3/
  - AU:   /Library/Audio/Plug-Ins/Components/
  - Docs: /Library/Audio/Documentation/Viral Samples/Call & Response MIDI/

Windows 64-bit (Setup.exe):
Run "Call & Response MIDI v2.0.0 Windows Setup.exe" to install:
  - VST3: C:\\Program Files\\Common Files\\VST3\\
  - App:  C:\\Program Files\\Viral Samples\\Call & Response MIDI\\
  - Docs: C:\\Program Files\\Viral Samples\\Call & Response MIDI\\Documentation\\

----------------------------------------------------------------------
QUICK KEY COMMANDS
----------------------------------------------------------------------
Save Preset to Slot A-H : Cmd+Click (or Shift/Alt/Ctrl+Click) on slot
Load Preset from Slot   : Double-Click on slot
Preset Context Menu     : Right-Click on slot
Continuous Swing Drag   : Click and drag vertically on SWING box
Continuous Velocity Drag: Click and drag vertically on VELOCITY box
Add/Delete Note         : Double-Click grid cell or note
Move Note               : Drag note center horizontally/vertically
Resize Note Gate        : Hover right edge of note (<->) and drag
Audition Step           : Click note (stops on mouse release)
Audition Pitch          : Click piano keys on left margin
Toggle Accent / Slide   : Right-Click for Accent; Alt-Click for Slide
Mute Lane               : Click MUTE on Call or Response lane
Lock Lane               : Click padlock icon on lane header
Offset Notes            : Click [ < ] or [ > ] buttons under rotaries

Copyright (c) 2026 Viral Samples. All rights reserved.
"""
    readme_txt_path1 = os.path.join(base_dir, "Read Me.txt")
    readme_txt_path2 = os.path.join(doc_dir, "Read Me.txt")
    for p in [readme_txt_path1, readme_txt_path2]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(txt_content)
    print("Generated Read Me TXTs.")

    # 4. Render PDFs with Headless Chrome
    chrome = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
    if os.path.exists(chrome):
        print("Rendering PDFs with Headless Chrome...")
        # User Manual PDF
        out_pdf_manual = os.path.join(base_dir, "User Manual.pdf")
        out_pdf_manual_alt = os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.pdf")
        out_pdf_manual_doc = os.path.join(doc_dir, "User Manual.pdf")
        
        subprocess.run([
            chrome, "--headless", "--disable-gpu",
            f"--print-to-pdf={out_pdf_manual}",
            "--no-pdf-header-footer",
            manual_html_path1
        ], check=True)
        subprocess.run(["cp", out_pdf_manual, out_pdf_manual_alt], check=True)
        subprocess.run(["cp", out_pdf_manual, out_pdf_manual_doc], check=True)
        print("Generated User Manual PDFs.")

        # Read Me PDF
        out_pdf_readme = os.path.join(base_dir, "Read Me.pdf")
        out_pdf_readme_doc = os.path.join(doc_dir, "Read Me.pdf")
        subprocess.run([
            chrome, "--headless", "--disable-gpu",
            f"--print-to-pdf={out_pdf_readme}",
            "--no-pdf-header-footer",
            readme_html_path1
        ], check=True)
        subprocess.run(["cp", out_pdf_readme, out_pdf_readme_doc], check=True)
        print("Generated Read Me PDFs.")
    else:
        print("Chrome not found for PDF generation.")

if __name__ == "__main__":
    generate_docs()
