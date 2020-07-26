/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** \file
 * \ingroup bgpencil
 */

#include <stdio.h>

#include "../gpencil_io_exporter.h"
#include "gpencil_io_svg.h"

using blender::io::gpencil::GpencilExporterSVG;

bool gpencil_io_export(const GpencilExportParams *params)
{
  switch (params->mode) {
    case GP_EXPORT_TO_SVG: {
      GpencilExporterSVG writter = GpencilExporterSVG(params);
      writter.write(std::string(params->frame));
      break;
    }
    default:
      break;
  }

  return true;
}
