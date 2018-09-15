export class MagickNode {

  public constructor(sourcePath: string)

  /**
   * Gets the current width of the image
   *
   * @type {number}
   * @memberof MagickNode
   */
  public width: number;

  /**
   * Gets the current height of the image
   *
   * @type {number}
   * @memberof MagickNode
   */
  public height: number;

  /**
   * Gets the original width of the image
   *
   * @type {number}
   * @memberof MagickNode
   */
  public baseWidth: number;

  /**
   * Gets the original height of the image
   *
   * @type {number}
   * @memberof MagickNode
   */
  public baseHeight: number;

  /**
   * Scale the image to a particular height and width. Aspect ratio is not preserved.
   *
   * @param {number} width The width to scale to
   * @param {number} height The height to scale to
   * @returns {this}
   * @memberof MagickNode
   */
  public scale(width: number, height: number): Promise<void>;

  /**
   * Scale to a particular width keeping the aspect ratio
   *
   * @param {number} width The width to scale to. The height will scale to preserve the aspect ratio.
   * @returns {this}
   * @memberof MagickNode
   */
  public scaleToWidth(width: number): Promise<void>;

  /**
   * Scale to a particular height keeping the aspect ratio
   *
   * @param {number} height The height to scale to. The width will scale to preserve the aspect ratio.
   * @returns {this}
   * @memberof MagickNode
   */
  public scaleToHeight(height: number): Promise<void>;

  public crop(width: number, height: number): Promise<void>;
  public crop(width: number, height: number, offsetX: number): Promise<void>;
  public crop(width: number, height: number, offsetX: number, offsetY: number): Promise<void>;

  public rotate(degrees: number): Promise<void>;

  public flip(direction: 'horizontal' | 'vertical'): Promise<void>;

  public save(path: string): Promise<void>;
  public save(path: string, quality: number): Promise<void>;

  /**
   * Gets the image as a buffer. By default the buffer is a png.
   *
   * @memberof MagickNode
   */
  public toBuffer(): Promise<Buffer>;

  /**
   * Gets the image as a buffer using the specified format.
   *
   * @param {string} format The buffer format 'png', 'jpg', 'gif', etc.
   * @memberof MagickNode
   */
  public toBuffer(format: string): Promise<Buffer>;

  /**
   * Gets the image as a buffer using the specified format and quality.
   *
   * @param {string} format The buffer format 'png', 'jpg', 'gif', etc.
   * @param {number} quality The quality between 1 and 100
   * @memberof MagickNode
   */
  public toBuffer(format: string, quality: number): Promise<Buffer>;
}