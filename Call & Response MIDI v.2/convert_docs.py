#!/usr/bin/env python3
import re
import html
import subprocess
import os

def clean_latex(text):
    t = text
    t = t.replace(r"\times", "×")
    t = t.replace(r"\pm", "±")
    t = t.replace(r"\sharp", "#")
    t = t.replace(r"\%", "%")
    t = t.replace(r"\,", " ")
    t = re.sub(r"\\text\{([^}]+)\}", r"\1", t)
    t = t.replace("$", "")
    return t

def md_to_html_body(md_text):
    lines = md_text.splitlines()
    html_lines = []
    in_list = False
    in_table = False
    in_code = False
    code_lines = []
    
    for line in lines:
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
                continue
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
    t = clean_latex(text)
    t = re.sub(r'\*\*\*(.*?)\*\*\*', r'<strong><em>\1</em></strong>', t)
    t = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', t)
    t = re.sub(r'\*(.*?)\*', r'<em>\1</em>', t)
    t = re.sub(r'`(.*?)`', r'<code>\1</code>', t)
    return t

# Dedicated High-Contrast Print / PDF Template
PDF_PRINT_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>{title}</title>
<style>
  @page {{
    size: A4 portrait;
    margin: 18mm 16mm 18mm 16mm;
  }}
  * {{ box-sizing: border-box; margin: 0; padding: 0; }}
  body {{
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
    background: #ffffff;
    color: #0f172a;
    font-size: 10pt;
    line-height: 1.55;
    -webkit-print-color-adjust: exact;
    print-color-adjust: exact;
  }}
  .container {{
    max-width: 100%;
    margin: 0 auto;
    background: #ffffff;
  }}
  h1 {{
    color: #0f172a;
    font-size: 1.85rem;
    font-weight: 800;
    margin-bottom: 8px;
    border-bottom: 2.5px solid #0284c7;
    padding-bottom: 8px;
    break-after: avoid;
    page-break-after: avoid;
  }}
  h2 {{
    color: #0f172a;
    font-size: 1.3rem;
    font-weight: 700;
    margin-top: 22px;
    margin-bottom: 10px;
    border-left: 4px solid #0284c7;
    padding-left: 10px;
    break-after: avoid;
    page-break-after: avoid;
  }}
  h3 {{
    color: #0369a1;
    font-size: 1.08rem;
    font-weight: 700;
    margin-top: 16px;
    margin-bottom: 6px;
    break-after: avoid;
    page-break-after: avoid;
  }}
  h4 {{
    color: #c2410c;
    font-size: 0.98rem;
    font-weight: 600;
    margin-top: 12px;
    margin-bottom: 4px;
    break-after: avoid;
    page-break-after: avoid;
  }}
  p {{
    margin-bottom: 10px;
    color: #1e293b;
    font-size: 10pt;
  }}
  strong {{
    color: #0f172a;
    font-weight: 700;
  }}
  em {{
    color: #334155;
  }}
  hr {{
    border: 0;
    height: 1px;
    background: #cbd5e1;
    margin: 18px 0;
  }}
  ul {{
    margin-left: 20px;
    margin-bottom: 12px;
  }}
  li {{
    margin-bottom: 4px;
    color: #1e293b;
  }}
  code {{
    background: #f1f5f9;
    color: #0369a1;
    padding: 1.5px 5px;
    border-radius: 4px;
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
    font-size: 0.88em;
    font-weight: 600;
    border: 1px solid #cbd5e1;
  }}
  pre {{
    background: #f8fafc;
    border: 1px solid #cbd5e1;
    border-radius: 6px;
    padding: 12px 16px;
    overflow-x: auto;
    margin-bottom: 14px;
    break-inside: avoid;
    page-break-inside: avoid;
  }}
  pre code {{
    background: transparent;
    border: none;
    padding: 0;
    color: #0f172a;
    font-size: 0.88em;
    font-weight: 400;
    display: block;
    line-height: 1.45;
  }}
  table {{
    width: 100%;
    border-collapse: collapse;
    margin: 14px 0;
    border: 1px solid #cbd5e1;
    border-radius: 4px;
    font-size: 9.2pt;
    break-inside: avoid;
    page-break-inside: avoid;
  }}
  th {{
    background: #f1f5f9;
    color: #0f172a;
    text-align: left;
    padding: 8px 12px;
    border: 1px solid #cbd5e1;
    border-bottom: 2px solid #94a3b8;
    font-weight: 700;
  }}
  td {{
    padding: 7px 12px;
    border: 1px solid #e2e8f0;
    color: #1e293b;
    vertical-align: top;
  }}
  tr:nth-child(even) td {{
    background: #f8fafc;
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

# Dark Mode Interactive HTML Template for Web Browsing
WEB_HTML_TEMPLATE = """<!DOCTYPE html>
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
  }}
  h2 {{
    color: #fff;
    font-size: 1.45rem;
    margin-top: 32px;
    margin-bottom: 14px;
    border-left: 4px solid var(--cyan);
    padding-left: 12px;
  }}
  h3 {{
    color: var(--cyan);
    font-size: 1.15rem;
    margin-top: 22px;
    margin-bottom: 8px;
  }}
  h4 {{
    color: var(--orange);
    font-size: 1.02rem;
    margin-top: 16px;
    margin-bottom: 6px;
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
    color: var(--text);
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
    color: var(--text);
  }}
  tr:nth-child(even) {{
    background: var(--table-stripe);
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
    release_dir = os.path.join(base_dir, "Viral Samples - Call & Response MIDI v2.0.0 (MAC:WIN)")
    os.makedirs(doc_dir, exist_ok=True)
    os.makedirs(release_dir, exist_ok=True)
    
    # 1. User Manual
    manual_md_path = os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.md")
    with open(manual_md_path, "r", encoding="utf-8") as f:
        manual_md = f.read()
    
    manual_body = md_to_html_body(manual_md)
    manual_web_html = WEB_HTML_TEMPLATE.format(title="Call & Response MIDI — User Manual (v2.0.0)", content=manual_body)
    manual_pdf_html = PDF_PRINT_TEMPLATE.format(title="Call & Response MIDI — User Manual (v2.0.0)", content=manual_body)
    
    for p in [
        os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.html"),
        os.path.join(doc_dir, "Call_and_Response_MIDI_User_Manual.html"),
        os.path.join(doc_dir, "User_Manual.html"),
        os.path.join(release_dir, "Call_and_Response_MIDI_User_Manual.html"),
    ]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(manual_web_html)
            
    manual_print_path = os.path.join(base_dir, "User_Manual_Print.html")
    with open(manual_print_path, "w", encoding="utf-8") as f:
        f.write(manual_pdf_html)
    print("Generated User Manual HTMLs.")

    # 2. Read Me
    readme_md_path = os.path.join(base_dir, "Read Me.md")
    with open(readme_md_path, "r", encoding="utf-8") as f:
        readme_md = f.read()
        
    readme_body = md_to_html_body(readme_md)
    readme_web_html = WEB_HTML_TEMPLATE.format(title="Call & Response MIDI — Read Me (v2.0.0)", content=readme_body)
    readme_pdf_html = PDF_PRINT_TEMPLATE.format(title="Call & Response MIDI — Read Me (v2.0.0)", content=readme_body)
    
    for p in [
        os.path.join(base_dir, "Read_Me.html"),
        os.path.join(doc_dir, "Read_Me.html"),
        os.path.join(release_dir, "Read_Me.html"),
    ]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(readme_web_html)
            
    readme_print_path = os.path.join(base_dir, "Read_Me_Print.html")
    with open(readme_print_path, "w", encoding="utf-8") as f:
        f.write(readme_pdf_html)
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
    for p in [
        os.path.join(base_dir, "Read Me.txt"),
        os.path.join(doc_dir, "Read Me.txt"),
        os.path.join(release_dir, "Read Me.txt"),
    ]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(txt_content)
    print("Generated Read Me TXTs.")

    # 4. Product Presentation
    pres_md_path = os.path.join(base_dir, "PRODUCT_PRESENTATION.md")
    pres_print_path = os.path.join(base_dir, "PRODUCT_PRESENTATION_Print.html")
    if os.path.exists(pres_md_path):
        with open(pres_md_path, "r", encoding="utf-8") as f:
            pres_md = f.read()
        pres_body = md_to_html_body(pres_md)
        pres_web_html = WEB_HTML_TEMPLATE.format(title="Call & Response MIDI — Product Presentation", content=pres_body)
        pres_pdf_html = PDF_PRINT_TEMPLATE.format(title="Call & Response MIDI — Product Presentation", content=pres_body)
        
        for p in [
            os.path.join(base_dir, "PRODUCT_PRESENTATION.html"),
            os.path.join(doc_dir, "PRODUCT_PRESENTATION.html"),
            os.path.join(release_dir, "PRODUCT_PRESENTATION.html"),
        ]:
            with open(p, "w", encoding="utf-8") as f:
                f.write(pres_web_html)
                
        with open(pres_print_path, "w", encoding="utf-8") as f:
            f.write(pres_pdf_html)
            
        # Sync TXT and MD
        pres_txt_path = os.path.join(base_dir, "PRODUCT_PRESENTATION.txt")
        if os.path.exists(pres_txt_path):
            with open(pres_txt_path, "r", encoding="utf-8") as f:
                pres_txt = f.read()
            for p in [os.path.join(doc_dir, "PRODUCT_PRESENTATION.txt"), os.path.join(release_dir, "PRODUCT_PRESENTATION.txt")]:
                with open(p, "w", encoding="utf-8") as f:
                    f.write(pres_txt)
                    
        for p in [os.path.join(doc_dir, "PRODUCT_PRESENTATION.md"), os.path.join(release_dir, "PRODUCT_PRESENTATION.md")]:
            with open(p, "w", encoding="utf-8") as f:
                f.write(pres_md)
        print("Generated Product Presentation HTML, TXT, and MD.")

    # 5. Sync Markdown files
    for p in [
        os.path.join(doc_dir, "Call_and_Response_MIDI_User_Manual.md"),
        os.path.join(release_dir, "Call_and_Response_MIDI_User_Manual.md"),
    ]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(manual_md)
            
    for p in [
        os.path.join(doc_dir, "Read Me.md"),
        os.path.join(release_dir, "Read Me.md"),
    ]:
        with open(p, "w", encoding="utf-8") as f:
            f.write(readme_md)

    # 6. Render PDFs with Headless Chrome from High-Contrast Print templates
    chrome = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
    if os.path.exists(chrome):
        print("Rendering High-Contrast PDFs with Headless Chrome...")
        out_pdf_manual = os.path.join(base_dir, "User Manual.pdf")
        
        subprocess.run([
            chrome, "--headless", "--disable-gpu",
            f"--print-to-pdf={out_pdf_manual}",
            "--no-pdf-header-footer",
            manual_print_path
        ], check=True)
        
        for p in [
            os.path.join(base_dir, "Call_and_Response_MIDI_User_Manual.pdf"),
            os.path.join(doc_dir, "User Manual.pdf"),
            os.path.join(release_dir, "User Manual.pdf"),
        ]:
            subprocess.run(["cp", out_pdf_manual, p], check=True)
        print("Generated High-Contrast User Manual PDFs.")

        out_pdf_readme = os.path.join(base_dir, "Read Me.pdf")
        subprocess.run([
            chrome, "--headless", "--disable-gpu",
            f"--print-to-pdf={out_pdf_readme}",
            "--no-pdf-header-footer",
            readme_print_path
        ], check=True)
        
        for p in [
            os.path.join(doc_dir, "Read Me.pdf"),
            os.path.join(release_dir, "Read Me.pdf"),
        ]:
            subprocess.run(["cp", out_pdf_readme, p], check=True)
        print("Generated High-Contrast Read Me PDFs.")
        
        if os.path.exists(pres_print_path):
            out_pdf_pres = os.path.join(base_dir, "PRODUCT_PRESENTATION.pdf")
            subprocess.run([
                chrome, "--headless", "--disable-gpu",
                f"--print-to-pdf={out_pdf_pres}",
                "--no-pdf-header-footer",
                pres_print_path
            ], check=True)
            for p in [
                os.path.join(doc_dir, "PRODUCT_PRESENTATION.pdf"),
                os.path.join(release_dir, "PRODUCT_PRESENTATION.pdf"),
            ]:
                subprocess.run(["cp", out_pdf_pres, p], check=True)
            print("Generated High-Contrast Product Presentation PDFs.")
        
        # Clean up temporary print html files
        if os.path.exists(manual_print_path): os.remove(manual_print_path)
        if os.path.exists(readme_print_path): os.remove(readme_print_path)
        if os.path.exists(pres_print_path): os.remove(pres_print_path)
    else:
        print("Chrome not found for PDF generation.")

if __name__ == "__main__":
    generate_docs()
