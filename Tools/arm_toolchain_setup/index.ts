import {finished} from "node:stream/promises";
import {Readable} from "node:stream";
import {SingleBar} from "cli-progress";
import {createHash} from "node:crypto";
import * as os from "node:os";
import path from "node:path";
import * as child_process from "node:child_process";
import * as fs from "node:fs";

(async function () {
    if (path.basename(process.cwd()) === "arm_toolchain_setup") {
        process.chdir("../..");
    }

    const toolchainDir = "toolchain"

    const supportedPlatforms = ["win32", "linux"] as const;
    type SupportedPlatforms = typeof supportedPlatforms[number];

    if (os.arch() != "x64") {
        throw Error("Auto toolchain setup is only supported on x84_64");
    }

    if (!supportedPlatforms.includes(os.platform() as any)) {
        throw Error("Auto toolchain setup is only supported on windows and linux");
    }
    const platform = os.platform() as SupportedPlatforms

    interface ToolDefinition {
        url: string;
        sha256: string;
        size: number;
        onDownload: (file: string) => void;
    }

    const platform_tools: Record<SupportedPlatforms, Record<["gcc", "openocd"][number], ToolDefinition>> = {
        win32: {
            gcc: {
                url: "https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-mingw-w64-x86_64-arm-none-eabi.zip",
                sha256: "864c0c8815857d68a1bbba2e5e2782255bb922845c71c97636004a3d74f60986",
                size: 290006806,
                onDownload(file) {
                    console.log("Extracting gcc...")

                    fs.mkdirSync("gcc")
                    process.chdir("gcc")
                    child_process.execFileSync("tar", ["-xf", path.join("..", file)]);
                    process.chdir("..")
                    fs.rmSync(file)
                }
            },
            openocd: {
                url: "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-win32-x64.zip",
                sha256: "6bfd3c97135aafef8affc9af1acf34fd0e2b9ca26044506f6abd7f95b7630052",
                size: 3225998,
                onDownload(file) {
                    console.log("Extracting openocd...")

                    const outputDir = file.substring(0, file.indexOf("-win32-x64.zip")) || file;

                    child_process.execFileSync("tar", ["-xf", file]);
                    fs.renameSync(outputDir, "openocd");
                    fs.rmSync(file)
                }
            }
        },
        linux: {
            gcc: {
                url: "https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz",
                // url: "http://localhost:8000/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz",
                sha256: "8f6903f8ceb084d9227b9ef991490413014d991874a1e34074443c2a72b14dbd",
                size: 149789432,
                onDownload(file) {
                    console.log("Extracting gcc...")

                    const outputDir = file.substring(0, file.indexOf(".tar")) || file;

                    child_process.execFileSync("tar", ["-xf", file]);
                    fs.renameSync(outputDir, "gcc");
                    fs.rmSync(file)
                }
            },
            openocd: {
                url: "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-linux-x64.tar.gz",
                // url: "http://localhost:8000/xpack-openocd-0.12.0-7-linux-x64.tar.gz",
                sha256: "94b3790983beaf8ed57e646c0620dd66d705fddae03d290823a6ed3b439468d6",
                size: 2802056,
                onDownload(file) {
                    console.log("Extracting openocd...")

                    const outputDir = file.substring(0, file.indexOf("-linux-x64.tar")) || file;

                    child_process.execFileSync("tar", ["-xf", file]);
                    fs.renameSync(outputDir, "openocd");
                    fs.rmSync(file)
                }
            }
        }
    }
    const tools = platform_tools[platform]

    console.log("--Resetting toolchain--")
    fs.rmSync(toolchainDir, {recursive: true, force: true})
    fs.mkdirSync(toolchainDir)
    process.chdir(toolchainDir)

    console.log("--Downloading tools--")
    for (const [tool, definition] of Object.entries(tools)) {
        const downloadedFileName = URL.parse(definition.url)!.pathname.match("[^/]+$")![0];
        const downloadedFilePath = downloadedFileName;

        console.log(`Downloading ${tool}...`);

        {
            const bar = new SingleBar({synchronousUpdate: false, etaBuffer: 100});
            bar.start(definition.size, 0);

            const webStream = Readable.fromWeb((await fetch(definition.url)).body!);
            const fileStream = fs.createWriteStream(downloadedFilePath, {flags: "wx"});
            const hashStream = createHash("sha256");
            webStream.pause();
            webStream.on("data", x => bar.increment(x.length));
            webStream.pipe(fileStream);
            webStream.pipe(hashStream);
            webStream.resume();
            await finished(fileStream);
            bar.stop();

            const hash = hashStream.setEncoding("hex").read() as string;
            if (hash != definition.sha256) {
                throw Error("Hash did not match expected value");
            }

            definition.onDownload(downloadedFileName);
        }
    }
})()